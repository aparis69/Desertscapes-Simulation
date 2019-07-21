#include "desert.h"

#include <iostream>
#include <fstream>

/*!
\brief Todo
*/
DuneSediment::DuneSediment()
{
	nx = ny = 256;
	box = Box2D(Vector2(0), 1);

	bedrock = ScalarField2D(nx, ny, box, 0.0);
	bedrockWeakness = ScalarField2D(nx, ny, box, 0.0);
	vegetation = ScalarField2D(nx, ny, box, 0.0);
	sediments = ScalarField2D(nx, ny, box, 0.0);

	matterToMove = 0.1f;
}

/*!
\brief Todo
*/
DuneSediment::DuneSediment(const Box2D& bbox, float rMin, float rMax)
{
	box = bbox;
	nx = ny = 256;

	bedrock = ScalarField2D(nx, ny, box, 0.0);
	bedrockWeakness = ScalarField2D(nx, ny, box, 0.0);
	vegetation = ScalarField2D(nx, ny, box, 0.0);
	sediments = ScalarField2D(nx, ny, box, 0.0);
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
			sediments.Set(i, j, Random::Uniform(rMin, rMax));
	}

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
			normals[id] = Normalize(Vector2(bedrock.Gradient(i, j) + sediments.Gradient(i, j)).ToVector3(-2.0f));
			vertices[id] = Vector3(
				box[0][0] + i * (box[1][0] - box[0][0]) / (nx - 1),
				box[0][1] + j * (box[1][1] - box[0][1]) / (ny - 1),
				Height(i, j)
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
		out << "vn " << normals.at(i).x << " " << normals.at(i).y << " " << normals.at(i).z << '\n';
	for (int i = 0; i < indices.size(); i += 3)
	{
		out << "f " << indices.at(i) + 1 << "//" << indices.at(i) + 1
			<< " " << indices.at(i + 1) + 1 << "//" << indices.at(i + 1) + 1
			<< " " << indices.at(i + 2) + 1 << "//" << indices.at(i + 2) + 1
			<< '\n';
	}
	out.close();
}
