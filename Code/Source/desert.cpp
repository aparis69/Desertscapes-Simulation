#include "desert.h"
#include "noise.h"

#include <iostream>
#include <fstream>

/*!
\brief Todo
*/
DuneSediment::DuneSediment()
{
	nx = ny = 256;
	box = Box2D(Vector2(0), 1);
	wind = Vector2(1, 0);

	bedrock = ScalarField2D(nx, ny, box, 0.0);
	vegetation = ScalarField2D(nx, ny, box, 0.0);
	sediments = ScalarField2D(nx, ny, box, 0.0);

	matterToMove = 0.1f;
	Vector2 celldiagonal = Vector2((box.TopRight()[0] - box.BottomLeft()[0]) / (nx - 1), (box.TopRight()[1] - box.BottomLeft()[1]) / (ny - 1));
	cellSize = Box2D(box.BottomLeft(), box.BottomLeft() + celldiagonal).Size().x; // We only consider squared heightfields
}

/*!
\brief Todo
*/
DuneSediment::DuneSediment(const Box2D& bbox, float rMin, float rMax, const Vector2& w)
{
	box = bbox;
	nx = ny = 256;
	wind = w;

	bedrock = ScalarField2D(nx, ny, box, 0.0);
	vegetation = ScalarField2D(nx, ny, box, 0.0);
	sediments = ScalarField2D(nx, ny, box, 0.0);
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			Vector2 p = bedrock.ArrayVertex(i, j);

			// Vegetation
			// Arbitrary clamped 2D noise - but you can use whatever you want.
			float v = PerlinNoise::fBm(Vector3(i * 7.91247f, j * 7.91247f, 0.0f), 1.0f, 0.002f, 3) / 1.75f;
			if (v > 0.58f)
				vegetation.Set(i, j, v);

			// Sand
			sediments.Set(i, j, Random::Uniform(rMin, rMax));
		}
	}

	// Debug code to write a ppm file showing the vegetation.
	// @Todo: could be refactored in the scalarfield2 class.
	//int i, j;
	//FILE* fp;
	//fopen_s(&fp, "first.ppm", "wb"); /* b - binary mode */
	//(void)fprintf(fp, "P6\n%d %d\n255\n", nx, ny);
	//for (j = 0; j < nx; ++j)
	//{
	//	for (i = 0; i < ny; ++i)
	//	{
	//		static unsigned char color[3];
	//		int v = (int)(vegetation.Get(i, j) * 256);
	//		color[0] = v % 256;  /* red */
	//		color[1] = v % 256;  /* green */
	//		color[2] = v % 256;  /* blue */
	//		(void)fwrite(color, 1, 3, fp);
	//	}
	//}
	//(void)fclose(fp);

	Vector2 celldiagonal = Vector2((box.TopRight()[0] - box.BottomLeft()[0]) / (nx - 1), (box.TopRight()[1] - box.BottomLeft()[1]) / (ny - 1));
	cellSize = Box2D(box.BottomLeft(), box.BottomLeft() + celldiagonal).Size().x; // We only consider squared heightfields
	
	matterToMove = 0.1f;
}

/*!
\brief Todo
*/
DuneSediment::~DuneSediment()
{

}

/*!
\brief Todo
*/
void DuneSediment::ExportObj(const std::string& url) const
{
	// Clear old data
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<int> indices;

	// Vertices & UVs & Normals
	normals.resize(nx * ny, Vector3(0));
	vertices.resize(nx * ny, Vector3(0));
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			int id = ToIndex1D(i, j);
			normals[id] = -Normalize(Vector2(bedrock.Gradient(i, j) + sediments.Gradient(i, j)).ToVector3(-2.0f));
			vertices[id] = Vector3(
				box[0][0] + i * (box[1][0] - box[0][0]) / (nx - 1),
				Height(i, j),
				box[0][1] + j * (box[1][1] - box[0][1]) / (ny - 1)
			);
		}
	}

	// Triangles
	int c = 0;
	int vertexArrayLength = ny * nx;
	while (c < vertexArrayLength - nx - 1)
	{
		if (c == 0 || (((c + 1) % nx != 0) && c <= vertexArrayLength - nx))
		{
			indices.push_back(c + nx + 1);
			indices.push_back(c + nx);
			indices.push_back(c);

			indices.push_back(c);
			indices.push_back(c + 1);
			indices.push_back(c + nx + 1);
		}
		c++;
	}

	// Export as .obj file
	std::ofstream out;	
	out.open(url);
	if (out.is_open() == false)
		return;
	out << "g " << "Obj" << std::endl;
	for (int i = 0; i < vertices.size(); i++)
		out << "v " << vertices.at(i).x << " " << vertices.at(i).y << " " << vertices.at(i).z << '\n';
	for (int i = 0; i < normals.size(); i++)
		out << "vn " << normals.at(i).x << " " << normals.at(i).z << " " << normals.at(i).y << '\n';
	for (int i = 0; i < indices.size(); i += 3)
	{
		out << "f " << indices.at(i) + 1 << "//" << indices.at(i) + 1
			<< " " << indices.at(i + 1) + 1 << "//" << indices.at(i + 1) + 1
			<< " " << indices.at(i + 2) + 1 << "//" << indices.at(i + 2) + 1
			<< '\n';
	}
	out.close();
}
