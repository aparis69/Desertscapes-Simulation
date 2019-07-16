#include "wtree.h"

/*!
\brief
*/
WNode::WNode() : box(Box2D()), e(1.0)
{
}

/*!
\brief
*/
WNode::WNode(const Box2D& box) : box(box), e(1.0)
{
}

/*!
\brief
*/
WNode::WNode(const Box2D& box, float ee) : box(box), e(ee)
{
}

/*!
\brief
*/
Vector2 WNode::Gradient(const Vector2& p, float t) const
{
	float epsilon = 1e-2f;
	float x = Intensity(Vector2(p[0] + epsilon, p[1]), t) - Intensity(Vector2(p[0] - epsilon, p[1]), t);
	float y = Intensity(Vector2(p[0], p[1] + epsilon), t) - Intensity(Vector2(p[0], p[1] - epsilon), t);
	return Vector2(x, y) / (2.0f * epsilon);
}

/*!
\brief
*/
Vector2 WNode::GradientOrthogonal(const Vector2& p, float t) const
{
	return Gradient(p, t); // .Orthogonal();
}

/*!
\brief
*/
float WNode::Energy() const
{
	return e;
}

/*!
\brief
*/
Box2D WNode::GetBox() const
{
	return box;
}

/*!
\brief
*/
void WNode::ChangeStrength(float ee)
{
	e = ee;
}
