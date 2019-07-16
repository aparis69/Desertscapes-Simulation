#include "wtree.h"

/*!
\class WUniform wtree.h
\brief
*/

/*!
\brief Constructor
\param bbox bounding box
\param dir wind direction in the primitive. Will be normalized.
\param ee norm of the wind vector, stored in the energy.
*/
WUniform::WUniform(const Box2D& bbox, const Vector2& dir, float ee) : WNode(bbox, ee)
{
	baseDir = Normalize(dir);
	ChangeDirection(baseDir);
}

/*!
\brief Computes intensity at a given point in space.
\param p point
*/
float WUniform::Intensity(const Vector2& p, float t) const
{
	if (!box.Contains(p))
		return 0.0;
	float u = 0.0f;
	//axis.R(p, u);
	//return e * box.Width() * 1.5 * u;
	return 0.0f;
}

/*!
\brief Compute the primitive parameters for a given direction.
*/
void WUniform::ChangeDirection(const Vector2& dir)
{
	//// Rotation case (clockwise/counterclockwise)
	//if (dir == Vector2(100, 0))
	//{
	//	double sin = sinf(Math::DegreeToRadian(10.0));
	//	double cos = cosf(Math::DegreeToRadian(10.0));
	//	double tx = baseDir[0];
	//	double ty = baseDir[1];
	//	baseDir[0] = (cos * tx) - (sin * ty);
	//	baseDir[1] = (sin * tx) + (cos * ty);
	//}
	//else if (dir == Vector2(-100, 0))
	//{
	//	double sin = sinf(Math::DegreeToRadian(-10.0));
	//	double cos = cosf(Math::DegreeToRadian(-10.0));
	//	double tx = baseDir[0];
	//	double ty = baseDir[1];
	//	baseDir[0] = (cos * tx) - (sin * ty);
	//	baseDir[1] = (sin * tx) + (cos * ty);
	//}
	//else
	//	baseDir = Normalize(dir);

	//// Counterclockwise direct orthogonal vector to compensate for the fact that the final wind
	//// Direction is obtained after a curl (clockwise orthogonal vector)
	//Vector2 d_orth = Vector2(baseDir[1], -baseDir[0]);
	//axis = Segment2(box.Center() - d_orth * box.Width(), box.Center() + d_orth * box.Width());
}

/*!
\brief
*/
void WUniform::Rotate(float t)
{
	//double sin = sinf(Math::DegreeToRadian(t));
	//double cos = cosf(Math::DegreeToRadian(t));
	//double tx = baseDir[0];
	//double ty = baseDir[1];
	//baseDir[0] = (cos * tx) - (sin * ty);
	//baseDir[1] = (sin * tx) + (cos * ty);

	//// Counterclockwise direct orthogonal vector to compensate for the fact that the final wind
	//// Direction is obtained after a curl (clockwise orthogonal vector)
	//Vector2 d_orth = Vector2(baseDir[1], -baseDir[0]);
	//axis = Segment2(box.Center() - d_orth * box.Width(), box.Center() + d_orth * box.Width());
}