#include "desert.h"

DuneSediment::DuneSediment()
{
	nx = ny = 256;
	box = Box2D(Vector2(0), 1);

	bedrock = ScalarField2D(nx, ny, box, 0.0);
	bedrockWeakness = ScalarField2D(nx, ny, box, 0.0);
	vegetation = ScalarField2D(nx, ny, box, 0.0);
	sediments = ScalarField2D(nx, ny, box, 0.0);
}

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
}

DuneSediment::~DuneSediment()
{

}
