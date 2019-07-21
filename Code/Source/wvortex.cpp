#include "wtree.h"

/*!
\class WVortex wtree.h
\brief
*/

/*!
\brief Constructor.
\param c center
\param rr radius
\param ee energy
*/
WVortex::WVortex(const Vector2& c, float rr, float ee) : WNode(Box2D(c, rr), ee), c(c), r(rr)
{
}

/*!
\brief Computes intensity at a given point in space.
\param p point
*/
float WVortex::Intensity(const Vector2& p, float t) const
{
	if (!box.Contains(p))
		return 0.0f;
	return e * box.Width() * Math::CubicSmoothCompact(SquaredMagnitude(p - c), r * r);
}
