#include "desert.h"

#include <algorithm>
#include <omp.h>

static Vector2i next8[8] = { Vector2i(1, 0), Vector2i(1, 1), Vector2i(0, 1), Vector2i(-1, 1), Vector2i(-1, 0), Vector2i(-1, -1), Vector2i(0, -1), Vector2i(1, -1) };
static float length8[8] = { 1.0f, sqrtf(2.0f), 1.0, sqrtf(2.0f), 1.0f, sqrtf(2.0f), 1.0f, sqrt(2.0f) };
static Vector2i Next(int i, int j, int k)
{
	return Vector2i(i, j) + next8[k];
}

/*!
\brief Compute the flow directions at a given point. Returns an integer representing the number of neighbour to distribute
the material to. Arrays nei and nslope contains respectively the neighbours in grid coordinates and the unit slopes.
\param p Point.
\param tanThresholdAngle tangent of the repose angle.
\param nei returned neighbour array.
\param nslope returned unit slope array.
*/
int DuneSediment::CheckSedimentFlowRelative(const Vector2i& p, float tanThresholdAngle, Vector2i* nei, float* nslope) const
{
	const float zp = Height(p.x, p.y);
	int n = 0;
	float slopesum = 0.0;
	for (int i = 0; i < 8; i++)
	{
		Vector2i b = Next(p.x, p.y, i);
		if (b.x < 0 || b.x >= nx || b.y < 0 || b.y >= ny)
			continue;
		float step = zp - Height(b.x, b.y);
		if (step > 0.0 && (step / cellSize * length8[i]) > tanThresholdAngle)
		{
			nei[n] = b;
			nslope[n] = step / length8[i];
			slopesum += nslope[n];
			n++;
		}
	}
	for (int k = 0; k < n; k++)
		nslope[k] = nslope[k] / slopesum;
	return n;
}

/*!
\brief Compute the flow directions at a given point. Returns an integer representing the number of neighbour to distribute
the material to. Arrays nei and nslope contains respectively the neighbours in grid coordinates and the unit slopes.
\param p Point.
\param tanThresholdAngle tangent of the repose angle.
\param nei returned neighbour array.
\param nslope returned unit slope array.
*/
int DuneSediment::CheckBedrockFlowRelative(const Vector2i& p, float tanThresholdAngle, Vector2i* nei, float* nslope) const
{
	const float zp = Bedrock(p.x, p.y);
	int n = 0;
	float slopesum = 0.0;
	for (int i = 0; i < 8; i++)
	{
		Vector2i b = Next(p.x, p.y, i);
		if (b.x < 0 || b.x >= nx || b.y < 0 || b.y >= ny)
			continue;
		float step = zp - Bedrock(b.x, b.y);
		if (step > 0.0 && (step / cellSize * length8[i]) > tanThresholdAngle)
		{
			nei[n] = b;
			nslope[n] = step / length8[i];
			slopesum += nslope[n];
			n++;
		}
	}
	for (int k = 0; k < n; k++)
		nslope[k] = nslope[k] / slopesum;
	return n;
}

/*!
\brief Stabilize a given grid vertex with the use of CheckSedimentFlowRelative() function.
Used by multi-thread functions, but can also be used in a single-thread context.
\param i x coordinate
\param j y coordinate
*/
void DuneSediment::StabilizeSedimentRelative(int i, int j)
{
	std::vector<Vector2i> queueToStabilize;
	Vector2i pts[8];
	float s[8];
	int n = 0;
	queueToStabilize.push_back(Vector2i(i, j));
	while (queueToStabilize.empty() == false)
	{
		Vector2i current = queueToStabilize[0];
		queueToStabilize.erase(queueToStabilize.begin());
		int id = ToIndex1D(current);
		if (sediments.Get(id) <= 0.0)
			continue;

		// Compute flow in all directions
		n = CheckSedimentFlowRelative(current, tanThresholdAngleSediment, pts, s);
		if (n == 0)
			continue;

		// Distribute to neighbours
		for (int a = 0; a < n; a++)
		{
			int nID = ToIndex1D(pts[a]);
#pragma omp atomic
			sediments[nID] += matterToMove * s[a];

			// Push neighbour to latter check stabilization
			queueToStabilize.push_back(pts[a]);
		}

		// Remove sediments from the current point
#pragma omp atomic
		sediments[id] -= matterToMove;
	}
}

/*!
\brief Todo
\param i
\param j
*/
bool DuneSediment::StabilizeBedrockRelative(int i, int j)
{
	std::vector<Vector2i> queueToStabilize;
	queueToStabilize.push_back(Vector2i(i, j));
	Vector2i pts[8];
	float s[8];
	int n = 0;
	bool stabilized = true;
	while (queueToStabilize.empty() == false)
	{
		Vector2i current = queueToStabilize[0];

		// Compute flow in all directions
		n = CheckBedrockFlowRelative(current, tanThresholdAngleBedrock, pts, s);
		if (n == 0)
		{
			queueToStabilize.erase(queueToStabilize.begin());
			continue;
		}
		stabilized = false;

		// Distribute to neighbours
		for (int a = 0; a < n; a++)
		{
			int nID = ToIndex1D(pts[a]);
#pragma omp atomic
			bedrock[nID] += matterToMove * s[a];

			// Push neighbour to latter check stabilization
			queueToStabilize.push_back(pts[a]);
		}

		// Remove sediments from the current point
#pragma omp atomic
		bedrock[ToIndex1D(current)] -= matterToMove;
	}
	return stabilized;
}

/*!
\brief Todo
*/
void DuneSediment::StabilizeBedrockAll()
{
	struct SortPredicate
	{
		DuneSediment* duneModel;
		inline SortPredicate(DuneSediment* d) : duneModel(d) { }

		inline bool operator()(Vector2i a, Vector2i b) const
		{
			return duneModel->Bedrock(a.x, a.y) < duneModel->Bedrock(b.x, b.y);
		}
	};
	std::vector<Vector2i> allPoints;
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
			allPoints.push_back(Vector2i(i, j));
	}
	std::sort(allPoints.begin(), allPoints.end(), SortPredicate(this));
	for (int i = 0; i < allPoints.size(); i++)
		StabilizeBedrockRelative(allPoints[i].x, allPoints[i].y);
}
