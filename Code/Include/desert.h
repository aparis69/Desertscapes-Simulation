#pragma once

#include "basics.h"

class DuneSediment
{
private:
	double tanThresholdAngleSediment = 0.60;		// ~33°
	double tanThresholdAngleWindShadowMin = 0.08;	// ~5°
	double tanThresholdAngleWindShadowMax = 0.26;	// ~15°
	double tanThresholdAngleBedrock = 2.5;			// ~68°

	bool vegetationOn = false;
	bool abrasionOn = false;

protected:
	ScalarField2D bedrock;			//!< Bedrock elevation layer, in meter.
	ScalarField2D sediments;		//!< Sediment elevation layer, in meter.
	ScalarField2D bedrockWeakness;	//!< Bedrock weakness layer between [0, 1].
	ScalarField2D vegetation;		//!< Vegetation presence map between [0, 1].

	Box2D box;						//!< World space bounding box.
	int nx, ny;						//!< Grid resolution.
	double matterToMove;			//!< Amount of sand transported by the wind, in meter.
	double cellSize;				//!< Size of one cell in meter, squared. Stored to speed up the simulation.

public:
	DuneSediment();
	DuneSediment(const Box2D& bbox, double rMin, double rMax);
	~DuneSediment();

	// Simulation
	void SimulationStepSingleThreadAtomic();
	void SimulationStepMultiThreadAtomic();
	void EndSimulationStep();
	void SimulationStepWorldSpace(int id);
	void VegetationStepAtomic(int id);
	void PerformReptationOnCell(int i, int j, int bounce, int id);
	void ComputeWindAtCell(int i, int j, Vector2& windDir) const;
	double IsInShadow(int i, int j, const Vector2& wind) const;
	void SnapWorld(Vector2& p) const;
	int CheckSedimentFlowRelative(const QPoint& p, double tanThresholdAngle, QPoint* nei, double* nslope) const;
	int CheckBedrockFlowRelative(const QPoint& p, double tanThresholdAngle, QPoint* nei, double * nslope) const;
	void StabilizeSedimentRelative(int i, int j);
	bool StabilizeBedrockRelative(int i, int j);
	void StabilizeBedrockAll();
	void PerformAbrasionOnCell(int i, int j, const Vector2& windDir);
	void SmoothBedrock();
	void SmoothSediments();

	// Exports & Meshing
	void ExportObj(const std::string& file) const;

	// Inlined functions and query
	double Height(int i, int j) const;
	double Height(const Vector2& p) const;
	double Bedrock(int i, int j) const;
	double Sediment(int i, int j) const;
	double Vegetation(int i, int j) const;
	void SetAbrasionMode(bool c);
	void SetVegetationMode(bool c);
	void SetHardnessData(const ScalarField2D& f);
	void SetVegetationData(const ScalarField2D& f);
	void SetBedrockData(const ScalarField2D& f);
	void SetSedimentData(const ScalarField2D& f);
};

/*!
\brief
*/
inline inline double DuneSediment::Height(int i, int j) const
{
	return bedrock.Get(i, j) + sediments.Get(i, j);
}

/*!
\brief
*/
inline double DuneSediment::Height(const Vector2& p) const 
{
	return bedrock.GetValueBilinear(p) + sediments.GetValueBilinear(p);
}

/*!
\brief
*/
inline double DuneSediment::Bedrock(int i, int j) const
{
	return bedrock.Get(i, j);
}

/*!
\brief
*/
inline double DuneSediment::Sediment(int i, int j) const
{
	return sediments.Get(i, j);
}

/*!
\brief
*/
inline double DuneSediment::Vegetation(int i, int j) const
{
	return vegetation.Get(i, j);
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
inline void DuneSediment::SetHardnessData(const ScalarField2D& f)
{
	bedrockWeakness = f;
}

/*!
\brief
*/
inline void DuneSediment::SetVegetationData(const ScalarField2D& f)
{
	vegetation = f;
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
