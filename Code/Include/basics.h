#pragma once
#include "vec.h"
#include <time.h>

#include <vector>

// Random (Dirty, C-style)
class Random
{
public:
	/*!
	\brief Constructor.
	*/
	Random()
	{
		// Empty
	}

	/*!
	\brief Compute a random number in a given range.
	\param a min
	\param b max
	*/
	static inline float Uniform(float a, float b)
	{
		return a + (b - a) * Uniform();
	}

	/*!
	\brief Compute a uniform random number in [0, 1]
	*/
	static inline float Uniform()
	{
		return float(rand()) / RAND_MAX;
	}

	/*!
	\brief Compute a random positive integer.
	*/
	static inline int Integer()
	{
		return rand();
	}
};

// AABB 2D class.
class Box2D
{
protected:
	Vector2 a;
	Vector2 b;

public:
	explicit Box2D();
	explicit Box2D(const Box2D& b1, const Box2D& b2);
	explicit Box2D(const Vector2& A, const Vector2& B);
	explicit Box2D(const Vector2& C, float R);

	float Width() const;
	Vector2 Size() const;
	Vector2 Vertex(int i) const;
	Vector2 Center() const;
	Vector2 BottomLeft() const;
	Vector2 TopRight() const;
	Vector2& operator[](int i);
	Vector2 operator[](int i) const;
};

/*
\brief Default Constructor
*/
inline Box2D::Box2D()
{
	a = Vector2(0);
	b = Vector2(0);
}

/*!
\brief Construct the box embedding the two box passed by parameter.
\param b1 first box
\param b2 second box
*/
inline Box2D::Box2D(const Box2D& b1, const Box2D& b2)
{
	a = Vector2::Min(b1.a, b2.a);
	b = Vector2::Max(b1.b, b2.b);
}

/*
\brief Constructor
\param A lower left vertex in world coordinates
\param B upper right vertex in world coordinates
*/
inline Box2D::Box2D(const Vector2& A, const Vector2& B) : a(A), b(B)
{
}

/*
\brief Constructor
\param C box center
\param R radius
*/
inline Box2D::Box2D(const Vector2& C, float R)
{
	Vector2 RR = Vector2(R);
	a = C - RR;
	b = C + RR;
}

/*
\brief Get one of the vertex of the box.
*/
inline Vector2 Box2D::Vertex(int i) const
{
	if (i == 0)
		return a;
	return b;
}

/*
\brief Compute the box center.
*/
inline Vector2 Box2D::Center() const
{
	return (a + b) / 2;
}

/*
\brief Get bottom left vertex in world coordinates
*/
inline Vector2 Box2D::BottomLeft() const
{
	return a;
}

/*
\brief Get top right vertex in world coordinates
*/
inline Vector2 Box2D::TopRight() const
{
	return b;
}

/*!
\brief Compute the size of the box.
*/
inline Vector2 Box2D::Size() const
{
	return b - a;
}

/*!
\brief Compute the width of a box.
\sa Box2D::Size()
*/
inline float Box2D::Width() const
{
	return b[0] - a[0];
}

/*
\brief Access box vertex by reference
*/
inline Vector2& Box2D::operator[](int i)
{
	if (i == 0)
		return a;
	return b;
}

/*
\brief Access box vertex by const value
*/
inline Vector2 Box2D::operator[](int i) const
{
	if (i == 0)
		return a;
	return b;
}


// ScalarField2D. Represents a 2D field (nx * ny) of scalar values bounded in world space. Can represent a heightfield.
class ScalarField2D
{
protected:
	Box2D box;
	int nx, ny;
	std::vector<float> values;

public:
	/*
	\brief Default Constructor
	*/
	inline ScalarField2D() : nx(0), ny(0)
	{
		// Empty
	}

	/*
	\brief Constructor
	\param nx size in x axis
	\param ny size in z axis
	\param bbox bounding box of the domain in world coordinates
	*/
	inline ScalarField2D(int nx, int ny, const Box2D& bbox) : box(bbox), nx(nx), ny(ny)
	{
		values.resize(size_t(nx * ny));
	}

	/*
	\brief Constructor
	\param nx size in x axis
	\param ny size in y axis
	\param bbox bounding box of the domain
	\param value default value of the field
	*/
	inline ScalarField2D(int nx, int ny, const Box2D& bbox, float value) : box(bbox), nx(nx), ny(ny)
	{
		values.resize(size_t(nx * ny));
		Fill(value);
	}

	/*
	\brief copy constructor
	\param field Scalarfield2D to copy
	*/
	inline ScalarField2D(const ScalarField2D& field) : ScalarField2D(field.nx, field.ny, field.box)
	{
		for (unsigned int i = 0; i < values.size(); i++)
			values[i] = field.values[i];
	}

	/*
	\brief Destructor
	*/
	inline ~ScalarField2D()
	{
	}

	/*
	\brief Compute the gradient for the vertex (i, j)
	*/
	inline Vector2 Gradient(int i, int j) const
	{
		Vector2 ret;
		float cellSizeX = (box.Vertex(1).x - box.Vertex(0).x) / (nx - 1);
		float cellSizeY = (box.Vertex(1).y - box.Vertex(0).y) / (ny - 1);

		// X Gradient
		if (i == 0)
			ret.x = (Get(i + 1, j) - Get(i, j)) / cellSizeX;
		else if (i == ny - 1)
			ret.x = (Get(i, j) - Get(i - 1, j)) / cellSizeX;
		else
			ret.x = (Get(i + 1, j) - Get(i - 1, j)) / (2.0f * cellSizeX);

		// Y Gradient
		if (j == 0)
			ret.y = (Get(i, j + 1) - Get(i, j)) / cellSizeY;
		else if (j == nx - 1)
			ret.y = (Get(i, j) - Get(i, j - 1)) / cellSizeY;
		else
			ret.y = (Get(i, j + 1) - Get(i, j - 1)) / (2.0f * cellSizeY);

		return ret;
	}

	/*
	\brief Normalize this field
	*/
	inline void NormalizeField()
	{
		float min = Min();
		float max = Max();
		for (int i = 0; i < ny * nx; i++)
			values[i] = (values[i] - min) / (max - min);
	}

	/*
	\brief Return the normalized version of this field
	*/
	inline ScalarField2D Normalized() const
	{
		ScalarField2D ret(*this);
		float min = Min();
		float max = Max();
		for (int i = 0; i < ny * nx; i++)
			ret.values[i] = (ret.values[i] - min) / (max - min);
		return ret;
	}

	/*!
	\brief Computes and returns the square root of the ScalarField.
	*/
	inline ScalarField2D Sqrt() const
	{
		ScalarField2D ret(*this);
		for (int i = 0; i < values.size(); i++)
			ret.values[i] = sqrt(ret.values[i]);
		return ret;
	}

	/*
	\brief Compute a vertex world position in 3D, with the scalar value treated as height.
	*/
	inline Vector3 Vertex(int i, int j) const
	{
		float x = box.Vertex(0).x + i * (box.Vertex(1).x - box.Vertex(0).x) / (nx - 1);
		float y = Get(i, j);
		float z = box.Vertex(0).y + j * (box.Vertex(1).y - box.Vertex(0).y) / (ny - 1);
		return Vector3(z, y, x);
	}

	/*!
	\brief Compute a vertex world position including in 2D.
	*/
	inline Vector2 ArrayVertex(int i, int j) const
	{
		float x = box.Vertex(0).x + i * (box.Vertex(1).x - box.Vertex(0).x) / (nx - 1);
		float z = box.Vertex(0).y + j * (box.Vertex(1).y - box.Vertex(0).y) / (ny - 1);
		return Vector2(z, x);
	}

	/*
	\brief Compute a vertex world position including his height.
	*/
	inline Vector3 Vertex(const Vector2i& v) const
	{
		float x = box.Vertex(0).x + v.x * (box.Vertex(1).x - box.Vertex(0).x) / (nx - 1);
		float y = Get(v.x, v.y);
		float z = box.Vertex(0).y + v.y * (box.Vertex(1).y - box.Vertex(0).y) / (ny - 1);
		return Vector3(z, y, x);
	}

	/*
	\brief Get Vertex world position by performing bilinear interpolation.
	\param v world position in 2D
	*/
	inline Vector3 Vertex(const Vector2& v) const
	{
		return Vector3(v.x, GetValueBilinear(v), v.y);
	}

	/*!
	\brief Check if a point lies inside the bounding box of the field.
	*/
	inline bool Inside(const Vector2& p) const
	{
		Vector2 q = p - box.Vertex(0);
		Vector2 d = box.Vertex(1) - box.Vertex(0);

		float u = q[0] / d[0];
		float v = q[1] / d[1];

		int j = int(u * (nx - 1));
		int i = int(v * (ny - 1));

		return Inside(i, j);
	}

	/*!
	\brief Check if a point lies inside the bounding box of the field.
	*/
	inline bool Inside(int i, int j) const
	{
		if (i < 0 || i >= nx || j < 0 || j >= ny)
			return false;
		return true;
	}

	/*!
	\brief Utility.
	*/
	inline void ToIndex2D(int index, int& i, int& j) const
	{
		i = index / nx;
		j = index % nx;
	}

	/*!
	\brief Utility.
	*/
	inline int ToIndex1D(const Vector2i& v) const
	{
		return v.x * nx + v.y;
	}

	/*!
	\brief Utility.
	*/
	inline int ToIndex1D(int i, int j) const
	{
		return i * nx + j;
	}

	/*!
	\brief Todo
	*/
	inline void CellInteger(const Vector2& p, int& i, int& j) const
	{
		Vector2 q = p - box.BottomLeft();
		Vector2 d = box.Size();

		float u = q[0] / d[0];
		float v = q[1] / d[1];

		// Scale
		u *= (nx - 1);
		v *= (ny - 1);

		// Integer coordinates
		i = int(v);
		j = int(u);
	}

	/*!
	\brief Returns the value of the field at a given coordinate.
	*/
	inline float Get(int row, int column) const
	{
		int index = ToIndex1D(row, column);
		return values[index];
	}

	/*!
	\brief Returns the value of the field at a given coordinate.
	*/
	inline float Get(int index) const
	{
		return values[index];
	}

	/*!
	\brief Returns the value of the field at a given coordinate.
	*/
	inline float Get(const Vector2i& v) const
	{
		int index = ToIndex1D(v);
		return values[index];
	}

	/*!
	\brief Todo
	*/
	void Add(int i, int j, float v)
	{
		values[ToIndex1D(i, j)] += v;
	}

	/*!
	\brief Todo
	*/
	void Remove(int i, int j, float v)
	{
		values[ToIndex1D(i, j)] -= v;
	}

	/*!
	\brief Todo
	*/
	void Add(const ScalarField2D& field)
	{
		for (int i = 0; i < values.size(); i++)
			values[i] += field.values[i];
	}

	/*!
	\brief Todo
	*/
	void Remove(const ScalarField2D& field)
	{
		for (int i = 0; i < values.size(); i++)
			values[i] -= field.values[i];
	}

	/*!
	\brief Compute the bilinear interpolation at a given world point.
	\param p world point.
	*/
	inline float GetValueBilinear(const Vector2& p) const
	{
		Vector2 q = p - box.Vertex(0);
		Vector2 d = box.Vertex(1) - box.Vertex(0);

		float texelX = 1.0f / float(nx - 1);
		float texelY = 1.0f / float(ny - 1);

		float u = q[0] / d[0];
		float v = q[1] / d[1];

		int i = int(v * (ny - 1));
		int j = int(u * (nx - 1));

		if (!Inside(i, j) || !Inside(i + 1, j + 1))
			return -1.0;

		float anchorU = j * texelX;
		float anchorV = i * texelY;

		float localU = (u - anchorU) / texelX;
		float localV = (v - anchorV) / texelY;

		float v1 = Get(i, j);
		float v2 = Get(i + 1, j);
		float v3 = Get(i + 1, j + 1);
		float v4 = Get(i, j + 1);

		return (1 - localU) * (1 - localV) * v1
			+ (1 - localU) * localV * v2
			+ localU * (1 - localV) * v4
			+ localU * localV * v3;
	}

	/*!
	\brief Fill all the field with a given value.
	*/
	inline void Fill(float v)
	{
		std::fill(values.begin(), values.end(), v);
	}

	/*!
	\brief Return the data in the field.
	\param c Index.
	*/
	inline float& operator[](int c)
	{
		return values[c];
	}

	/*!
	\brief Set a given value at a given coordinate.
	*/
	inline void Set(int row, int column, float v)
	{
		values[ToIndex1D(row, column)] = v;
	}

	/*!
	\brief Set a given value at a given coordinate.
	*/
	inline void Set(int index, float v)
	{
		values[index] = v;
	}

	/*!
	\brief Todo
	*/
	inline void ThresholdInferior(float t, float v)
	{
		for (int i = 0; i < values.size(); i++)
		{
			if (values[i] <= t)
				values[i] = v;
		}
	}

	/*!
	\brief Compute the maximum of the field.
	*/
	inline float Max() const
	{
		if (values.size() == 0)
			return 0.0f;
		float max = values[0];
		for (int i = 1; i < values.size(); i++)
		{
			if (values[i] > max)
				max = values[i];
		}
		return max;
	}

	/*!
	\brief Compute the minimum of the field.
	*/
	inline float Min() const
	{
		if (values.size() == 0)
			return 0.0f;
		float min = values[0];
		for (int i = 1; i < values.size(); i++)
		{
			if (values[i] < min)
				min = values[i];
		}
		return min;
	}

	/*!
	\brief Compute the average value of the scalarfield.
	*/
	inline float Average() const
	{
		float sum = 0.0f;
		for (int i = 0; i < values.size(); i++)
			sum += values[i];
		return sum / values.size();
	}

	/*!
	\brief Returns the size of x-axis of the array.
	*/
	inline int SizeX() const
	{
		return nx;
	}

	/*!
	\brief Returns the size of y-axis of the array.
	*/
	inline int SizeY() const
	{
		return ny;
	}

	/*!
	\brief Returns the bottom left corner of the bounding box.
	*/
	inline Vector2 BottomLeft() const
	{
		return box.Vertex(0);
	}

	/*!
	\brief Returns the top right corner of the bounding box.
	*/
	inline Vector2 TopRight() const
	{
		return box.Vertex(1);
	}

	/*!
	\brief Returns the bounding box of the field.
	*/
	inline Box2D GetBox() const
	{
		return box;
	}

	/*!
	\brief Compute the memory used by the field.
	*/
	inline int Memory() const
	{
		return sizeof(ScalarField2D) + sizeof(float) * int(values.size());
	}
};
