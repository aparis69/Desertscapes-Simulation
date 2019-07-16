#include "wtree.h"

/*!
\brief
*/
WBlend::WBlend(WNode* e1, WNode* e2)
{
	box = Box2D(e1->GetBox(), e2->GetBox());
	e[0] = e1;
	e[1] = e2;
}

/*!
\brief
*/
float WBlend::Intensity(const Vector2& p, float t) const
{
	return e[0]->Intensity(p, t) + e[1]->Intensity(p, t);
}

/*!
\brief
*/
void WBlend::ChangeStrength(float ee)
{
	e[0]->ChangeStrength(ee);
	e[1]->ChangeStrength(ee);
}

/*!
\brief
*/
void WBlend::ChangeDirection(const Vector2& dir)
{
	e[0]->ChangeDirection(dir);
	e[1]->ChangeDirection(dir);
}

/*!
\brief
*/
float WBlend::Energy() const
{
	return e[0]->Energy() + e[1]->Energy();
}
