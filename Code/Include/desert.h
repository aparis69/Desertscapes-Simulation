#pragma once

#include "basics.h"

class DuneSediment
{
private:
	float tanThresholdAngleSediment = 0.60f;		// ~33°
	float tanThresholdAngleWindShadowMin = 0.08f;	// ~5°
	float tanThresholdAngleWindShadowMax = 0.26f;	// ~15°
	float tanThresholdAngleBedrock = 2.5f;			// ~68°

	bool vegetationOn = false;
	bool abrasionOn = false;

protected:
	ScalarField2D bedrock;			//!< Bedrock elevation layer, in meter.
	ScalarField2D sediments;		//!< Sediment elevation layer, in meter.
	ScalarField2D vegetation;		//!< Vegetation presence in [0, 1].

	Box2D box;						//!< World space bounding box.
	int nx, ny;						//!< Grid resolution.
	float matterToMove;				//!< Amount of sand transported by the wind, in meter.
	float cellSize;					//!< Size of one cell in meter, squared. Stored to speed up the simulation.
	Vector2 wind;					//!< Base wind direction.

public:
	DuneSediment();
	DuneSediment(const Box2D& bbox, float rMin, float rMax, const Vector2& w);
	~DuneSediment();

	// Simulation
	int ToIndex1D(const Vector2i& q) const;
	int ToIndex1D(int i, int j) const;
	void SimulationStepMultiThreadAtomic();
	void EndSimulationStep();
	void SimulationStepWorldSpace();
	void PerformReptationOnCell(int i, int j, int bounce);
	void ComputeWindAtCell(int i, int j, Vector2& windDir) const;
	float IsInShadow(int i, int j, const Vector2& wind) const;
	void SnapWorld(Vector2& p) const;
	int CheckSedimentFlowRelative(const Vector2i& p, float tanThresholdAngle, Vector2i* nei, float* nslope) const;
	int CheckBedrockFlowRelative(const Vector2i& p, float tanThresholdAngle, Vector2i* nei, float * nslope) const;
	void StabilizeSedimentRelative(int i, int j);
	bool StabilizeBedrockRelative(int i, int j);
	void StabilizeBedrockAll();
	void PerformAbrasionOnCell(int i, int j, const Vector2& windDir);

	// Exports & Meshing
	void ExportObj(const std::string& file) const;
	void ExportObj(std::vector<Vector3>& vertex, std::vector<Vector3>& normals, std::vector<Vector3>& colors, std::vector<int>& indices) const;

	// Inlined functions and query
	float Height(int i, int j) const;
	float Height(const Vector2& p) const;
	float Bedrock(int i, int j) const;
	float Sediment(int i, int j) const;
	void SetAbrasionMode(bool c);
	void SetVegetationMode(bool c);
	void SetBedrockData(const ScalarField2D& f);
	void SetSedimentData(const ScalarField2D& f);
};

/*!
\brief Compute the 1D index from a given grid vertex.
\param q grid vertex.
*/
inline int DuneSediment::ToIndex1D(int i, int j) const
{
	return bedrock.ToIndex1D(i, j);
}

/*!
\brief Compute the 1D index from a given grid vertex.
\param q grid vertex.
*/
inline int DuneSediment::ToIndex1D(const Vector2i& q) const
{
	return bedrock.ToIndex1D(q);
}

/*!
\brief
*/
inline float DuneSediment::Height(int i, int j) const
{
	return bedrock.Get(i, j) + sediments.Get(i, j);
}

/*!
\brief
*/
inline float DuneSediment::Height(const Vector2& p) const 
{
	return bedrock.GetValueBilinear(p) + sediments.GetValueBilinear(p);
}

/*!
\brief
*/
inline float DuneSediment::Bedrock(int i, int j) const
{
	return bedrock.Get(i, j);
}

/*!
\brief
*/
inline float DuneSediment::Sediment(int i, int j) const
{
	return sediments.Get(i, j);
}

/*!
\brief
*/
inline void DuneSediment::SetAbrasionMode(bool c)
{
	abrasionOn = c;
}

/*!
\brief
*/
inline void DuneSediment::SetVegetationMode(bool c)
{
	vegetationOn = c;
}

/*!
\brief
*/
inline void DuneSediment::SetBedrockData(const ScalarField2D& f)
{
	bedrock = f;
}

/*!
\brief
*/
inline void DuneSediment::SetSedimentData(const ScalarField2D& f)
{
	sediments = f;
}
