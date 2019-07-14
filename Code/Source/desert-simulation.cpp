#include "desert.h"

#include <assert.h>
#include <omp.h>

// File scope variables
#define OMP_NUM_THREAD 8

static size_t abrasionStepCount = 0;
static float abrasionEpsilon = 0.5;
static Vector2i next8[8] = { Vector2i(1, 0), Vector2i(1, 1), Vector2i(0, 1), Vector2i(-1, 1), Vector2i(-1, 0), Vector2i(-1, -1), Vector2i(0, -1), Vector2i(1, -1) };
static Vector2i Next(int i, int j, int k)
{
	return Vector2i(i, j) + next8[k];
}
static int simulationStepCount = 0;

/*!
\brief Performs a multi-threaded simulation simulationStepCount.
*/
void DuneSediment::SimulationStepSingleThreadAtomic()
{
	int id = omp_get_thread_num();
	for (int a = 0; a < nx; a++)
	{
		for (int b = 0; b < ny; b++)
			SimulationStepWorldSpace(id);
	}
	EndSimulationStep();
}

/*!
\brief Todo
*/
void DuneSediment::SimulationStepMultiThreadAtomic()
{
#pragma omp parallel num_threads(OMP_NUM_THREAD)
	{
		int id = omp_get_thread_num();
#pragma omp for
		for (int a = 0; a < nx; a++)
		{
			for (int b = 0; b < ny; b++)
				SimulationStepWorldSpace(id);
		}
	}

	EndSimulationStep();
}

/*!
\brief Some operations are performed every five iteration
to save computation time.
*/
void DuneSediment::EndSimulationStep()
{
#pragma omp atomic
	simulationStepCount++;

	int mod = simulationStepCount % 5;
	if (mod == 0)
	{
		// Bedrock stabilization
		if (abrasionOn)
			StabilizeBedrockAll();

		// Update terrain data
		//wtree->SetTerrainData(ScalarField2(GetHeightField()));
		//wtree->SetTerrainData(bedrock);
	}
}

/*!
\brief Todo
*/
void DuneSediment::SimulationStepWorldSpace(int id)
{
	Vector2 windDir;

	// (1) Select a random grid position (Lifting)
	int startI = Random::Integer() % nx;
	int startJ = Random::Integer() % ny;
	int start1D = ToIndex1D(startI, startJ);

	// Compute wind at start cell
	ComputeWindAtCell(startI, startJ, windDir);

	// No sediment to move
	if (sediments.Get(start1D) <= 0.0)
		return;
	// Wind shadowing probability
	if (Random::Uniform() < IsInShadow(startI, startJ, windDir))
	{
		StabilizeSedimentRelative(startI, startJ);
		return;
	}
	// Vegetation can retain sediments in the lifting process
	if (Random::Uniform() < vegetation[start1D])
	{
		StabilizeSedimentRelative(startI, startJ);
		return;
	}

	// (2) Lift grain at start cell
#pragma omp atomic
	sediments[start1D] -= matterToMove;

	// (3) Jump downwind by saltation hop length (wind direction). Repeat until sand is deposited.
	int destI = startI;
	int destJ = startJ;
	Vector2 pos = bedrock.ArrayVertex(destI, destJ);
	int bounce = 0;
	while (true)
	{
		// Compute wind at the current cell
		ComputeWindAtCell(destI, destJ, windDir);

		// Compute new world position and new grid position (after wind addition)
		pos = pos + windDir;
		SnapWorld(pos);
		bedrock.CellInteger(pos, destI, destJ);

		// Conversion to 1D index to speed up computation
		int destID = ToIndex1D(destI, destJ);

		// Abrasion of the bedrock occurs with low sand supply, weak bedrock and a low probability.
		if (abrasionOn && Random::Uniform() < 0.2 && sediments.Get(destID) < 0.5)
			PerformAbrasionOnCell(destI, destJ, windDir);

		// Probability of deposition
		float p = Random::Uniform();

		// Shadowed cell
		if (p < IsInShadow(destI, destJ, windDir))
		{
#pragma omp atomic
			sediments[destID] += matterToMove;
			break;
		}
		// Sandy cell - 60% chance of deposition (if vegetation == 0.0)
		else if (sediments.Get(destID) > 0.0 && p < 0.6 + (vegetation.Get(destID) * 0.4))
		{
#pragma omp atomic
			sediments[destID] += matterToMove;
			break;
		}
		// Empty cell - 40% chance of deposition (if vegetation == 0.0)
		else if (sediments.Get(destID) <= 0.0 && p < 0.4 + (vegetation.Get(destID) * 0.6))
		{
#pragma omp atomic
			sediments[destID] += matterToMove;
			break;
		}

		// Perform reptation at each bounce
		bounce++;
		if (Random::Uniform() < 1.0 - vegetation[start1D])
			PerformReptationOnCell(destI, destJ, bounce, id);
	}
	// End of the deposition loop - we have move matter from (startI, startJ) to (destI, destJ)

	// Perform reptation at the deposition simulationStepCount
	if (Random::Uniform() < 1.0 - vegetation[start1D])
		PerformReptationOnCell(destI, destJ, bounce, id);

	// (4) Check for the angle of repose on the original cell
	StabilizeSedimentRelative(startI, startJ);

	// (5) Check for the angle of repose on the destination cell if different
	StabilizeSedimentRelative(destI, destJ);
}

/*!
\brief Todo
*/
void DuneSediment::PerformReptationOnCell(int i, int j, int bounce, int id)
{
	// Compute amount of sand to creep ; function of number of bounce.
	int b = Math::Clamp(bounce, 0, 3);
	float t = float(b) / 3.0f;
	float se = Math::Lerp(matterToMove / 2.0f, matterToMove, t);
	float rReptationSquared = 2.0 * 2.0;
	Vector2 p = bedrock.ArrayVertex(i, j);

	// Distribute sand at the 2-steepest neighbours
	Vector2i nei[8];
	float nslope[8];
	int n = Math::Min(2, CheckSedimentFlowRelative(Vector2i(i, j), tanThresholdAngleSediment, nei, nslope));
	int nEffective = 0;
	for (int k = 0; k < n; k++)
	{
		Vector2i next = nei[k];
		float sei = se / n;

		// We don't perform reptation if the grid discretization is too low.
		Vector2 pk = bedrock.ArrayVertex(next.x, next.y);
		if (SquaredMagnitude(p - pk) > rReptationSquared)
			continue;

		// Distribute sediment to neighbour
#pragma omp atomic
		sediments[ToIndex1D(next)] += sei;

		// Count the amount of neighbour which received sand from the current cell (i, j)
		nEffective++;
	}

	// Remove sediment at the current cell
	if (n > 0 && nEffective > 0)
	{
#pragma omp atomic
		sediments[ToIndex1D(i, j)] -= se;
	}
}

/*!
\brief
*/
void DuneSediment::ComputeWindAtCell(int i, int j, Vector2& windDir) const
{
#ifdef _DEBUG
	assert(Math::IsNumber(i));
	assert(Math::IsNumber(j));
#endif

	// Compute the base wind vector at this position
	windDir = Vector2(3, 0); // wtree->Direction(bedrock.ArrayVertex(i, j));

	// Modulate wind strength with sediment layer: increase -velocity on slope in the direction of the wind
	Vector2 g = sediments.Gradient(i, j);
	float similarity = 0.0f;
	float slope = 0.0f;
	if (g != Vector2(0.0f) && windDir != Vector2(0.0f))
	{
		similarity = Math::Clamp(Dot(Normalize(g), Normalize(windDir)));
		slope = Math::Clamp(Magnitude(g));
	}

	// Wind velocity is floatd in the best case
	float t = (similarity + slope) / 2.0f;
	windDir = Math::Lerp(windDir, windDir * 2.0f, t);

#ifdef _DEBUG
	assert(Math::IsNumber(windDir[0]));
	assert(Math::IsNumber(windDir[1]));
#endif
}

/*!
\brief Todo
*/
void DuneSediment::PerformAbrasionOnCell(int i, int j, const Vector2& windDir)
{
	int id = ToIndex1D(i, j);

	// Vegetation protects from abrasion
	float v = vegetation.Get(id);

	// Bedrock resistance (1.0 equals to weak, 0.0 equals to hard)
	float h = bedrockWeakness.Get(id);

	// Wind strength
	float w = Math::Clamp(Magnitude(windDir), 0.0f, 2.0f);

	float si = abrasionEpsilon * (1.0f - v) * (1.0f - h) * w;
	if (si == 0.0)
		return;

	// Transform bedrock into dust (nothing)
#pragma omp atomic
	bedrock[id] -= si;

#pragma omp atomic
	abrasionStepCount++;
}

/*!
\brief Check if a given grid vertex is in the wind shadow.
Use the threshold angle described in geomorphology papers, ie. ~[5, 15]°.
\param i x coordinate
\param j y coordinate
\param unitWindDir unit wind direction.
\returns true of the vertex is in shadow, false otherwise.
*/
float DuneSediment::IsInShadow(int i, int j, const Vector2& windDir) const
{
	const float windStepLength = 1.0;
	const Vector2 windStep = Vector2(
		windDir[0] > 0.0f ? windStepLength : windDir[0] < 0.0f ? -windStepLength : 0.0f,
		windDir[1] > 0.0f ? windStepLength : windDir[1] < 0.0f ? -windStepLength : 0.0f
	);
	Vector2 p = bedrock.ArrayVertex(i, j);
	Vector2 pShadow = p;
	float rShadow = 10.0f;
	float hp = Height(p);
	float ret = 0.0;
	while (true)
	{
		pShadow = pShadow - windStep;
		if (pShadow == p)
			break;
		Vector2 pShadowSnapped = pShadow;
		SnapWorld(pShadowSnapped);

		float d = Magnitude(p - pShadow);
		if (d > rShadow)
			break;

		float step = Height(pShadowSnapped) - hp;
		float t = (step / d);
		float s = Math::Step(t, tanThresholdAngleWindShadowMin, tanThresholdAngleWindShadowMax);
		ret = Math::Max(ret, s);
	}
	return ret;
}

/*!
\brief Todo
*/
void DuneSediment::SnapWorld(Vector2& p) const
{
	if (p[0] < 0)
		p[0] = box.Size()[0] + p[0];
	else if (p[0] >= box.Size()[0])
		p[0] = p[0] - box.Size()[0];

	if (p[1] < 0)
		p[1] = box.Size()[1] + p[1];
	else if (p[1] >= box.Size()[1])
		p[1] = p[1] - box.Size()[1];

#ifdef _DEBUG
	assert(bedrock.Inside(p));
#endif
}
