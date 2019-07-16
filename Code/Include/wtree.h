#pragma once
#include "basics.h"

class WNode
{
protected:
	Box2D box;	//!< Bounding box of the primitive.
	float e;	//!< GetWindStrength of the primitive.

public:
	WNode();
	WNode(const Box2D& box);
	WNode(const Box2D& box, float ee);

	virtual Vector2 Gradient(const Vector2& p, float t = 0.0) const;
	virtual Vector2 GradientOrthogonal(const Vector2& p, float t = 0.0) const;
	virtual float Intensity(const Vector2& p, float t = 0.0) const = 0;

	virtual void ChangeStrength(float ee);
	virtual void ChangeDirection(const Vector2& dir) { }
	virtual void Rotate(float t) { }
	virtual void ComputeFromTerrain(const ScalarField2D& T) { }
	virtual float Energy() const;
	Box2D GetBox() const;
};

class WUniform : public WNode
{
private:
	Vector2 baseDir;	//!< Final wind direction.
	//Segment2 axis;		//!< Interpolation axis.

public:
	WUniform(const Box2D& bbox, const Vector2& dir, float e);
	float Intensity(const Vector2& p, float t = 0.0) const;

	virtual void Rotate(float t);
	virtual void ChangeDirection(const Vector2& dir);
};

class WVortex : public WNode
{
private:
	Vector2 c;	//!< Vortex center.
	float r;	//!< Vortex radius.

public:
	WVortex(const Vector2& c, float rr, float ee);
	float Intensity(const Vector2& p, float t = 0.0) const;
};

class WBlend : public WNode
{
protected:
	WNode* e[2];	//!< Child nodes.

public:
	WBlend(WNode* e1, WNode* e2);
	float Intensity(const Vector2& p, float t = 0.0) const;

	virtual void ChangeStrength(float ee);
	virtual void ChangeDirection(const Vector2& dir);
	virtual float Energy() const;
};

class WTree
{
private:
	WNode* root;			//!< Root node of the current tree.
	ScalarField2D hf[2];	//!< Terrain data.
	float K_deviation[2];	//!< Deviation amount per terrain resolution.
	float K_blend[2];		//!< Blending coefficient per terrain resolution.

public:
	WTree();
	WTree(WNode* r);
	WTree(WNode* r, const ScalarField2D& hf);

	// Queries
	float Intensity(const Vector2& p, float t = 0.0) const;
	Vector2 Direction(const Vector2& p, float t = 0.0) const;
	void AdaptToTerrain(const Vector2& p, Vector2& d) const;

	// Methods
	void Blend(WNode* e);
	void ChangeDirection(const Vector2& d);
	void ChangeStrength(float e);
	void Rotate(float t);

	// Getter
	void SetTerrainData(const ScalarField2D& hf);
	ScalarField2D GetTerrainData() const;
};
