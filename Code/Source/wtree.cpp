#include "wtree.h"

/*!
\class WTree wtree.h
\brief
*/

/*!
\brief Default constructor.
*/
WTree::WTree()
{
}

/*!
\brief Constructor
\param r root node
*/
WTree::WTree(WNode* r) : root(r)
{
}

/*!
\brief
*/
WTree::WTree(WNode* r, const ScalarField2D& hfData) : root(r)
{
	hf[0] = hfData;
	hf[1] = hfData;

	/*hf[0].Blur(int(200.0 / hfData.CellDiagonal()[0]));
	K_deviation[0] = 30.0;
	K_blend[0] = 0.8;

	hf[1].Blur(int(100.0 / hfData.CellDiagonal()[0]));
	K_deviation[1] = 2.0;
	K_blend[1] = 0.2;*/
}

/*!
\brief Computes intensity at a given point in space.
\param p point
*/
float WTree::Intensity(const Vector2& p, float t) const
{
	return root->Intensity(p, t);
}

/*!
\brief Blend the root node with a given node.
\param e node to blend with
*/
void WTree::Blend(WNode* e)
{
	root = new WBlend(root, e);
}

/*!
\brief Computes the wind direction at a given point in space.
Direction is defined as the clockwise orthogonal vector to the gradient.
\param p
*/
Vector2 WTree::Direction(const Vector2& p, float t) const
{
	Vector2 d = root->GradientOrthogonal(p, t);
	AdaptToTerrain(p, d);
	return d;
}

/*!
\brief
*/
void WTree::AdaptToTerrain(const Vector2& p, Vector2& d) const
{
	// Adapts the direction d at p with all terrain data (multiple resolution)
	Vector2 dd[2];
	int i = 0, j = 0;
	//hf[0].VertexInteger(p, i, j);
	//if (!hf[0].InsideVertexIndex(i, j))
	//	return;

	// Second, adapt wind strength to terrain (Venturi Effect), using smoothed heightfields
	float dNorm = Magnitude(d);
	float SS[2];
	float K_venturi = 0.005f;
	for (int k = 0; k < 2; k++)
	{
		float Z = hf[k].Get(i, j);
		SS[k] = dNorm + K_venturi * Z;
	}
	float S = SS[0] * K_blend[0] + SS[1] * K_blend[1];

	// First compute direction
	for (int k = 0; k < 2; k++)
	{
		Vector2 N_xy = -hf[k].Gradient(i, j);
		float Norm_N_xy = Magnitude(N_xy);
		float ss = Math::CubicSmoothCompact(Math::Sqr(Norm_N_xy), 1.0);

		// Deviation with an orthogonal vector to N_xy
		Vector2 N_xy_orth; // = N_xy.Orthogonal();
		if (Dot(N_xy_orth, d) < 0.0f)
			N_xy_orth = -N_xy_orth;
		dd[k] = ss * d + (1.0f - ss) * K_deviation[k] * N_xy_orth;
	}
	// Blend the directions computed from all terrain resolution
	d = Normalize(K_blend[0] * dd[0] + K_blend[1] * dd[1]);

	// Final direction
	d = S * d;

	// Very small values create errors in the simulation... No dunes form.
	if (Math::Abs(d[0]) < 10e-4) d[0] = 0.0f;
	if (Math::Abs(d[1]) < 10e-4) d[1] = 0.0f;
}

/*!
\brief
*/
void WTree::SetTerrainData(const ScalarField2D& hfData)
{
	hf[0] = hfData;
	hf[1] = hfData;

	//hf[0].Blur(int(200.0 / hfData.CellDiagonal()[0]));
	//K_deviation[0] = 30.0;
	//K_blend[0] = 0.5;

	//hf[1].Blur(int(100.0 / hfData.CellDiagonal()[0]));
	//K_deviation[1] = 10.0;
	//K_blend[1] = 0.5;
}

/*!
\brief
*/
ScalarField2D WTree::GetTerrainData() const
{
	return hf[0];
}

/*!
\brief
*/
void WTree::ChangeDirection(const Vector2& d)
{
	root->ChangeDirection(d);
}

/*!
\brief
*/
void WTree::ChangeStrength(float e)
{
	root->ChangeStrength(e);
}

/*!
\brief
*/
void WTree::Rotate(float t)
{
	root->Rotate(t);
}
