/*
	This is an example implementation of some of the results described in the paper "Desertscapes Simulation".

	No real time visualization in order to reduce dependencies. Running the program will output 3 .obj files which
	can then be visualized in another application (Blender, MeshLab).

	If you have any questions, you can contact me at:
	axel(dot)paris(at)liris(dot)cnrs(dot)fr
*/

#include "desert.h"

/*!
\brief
*/
void ExportScenes()
{
	// Transverse dunes are created under unimodal wind, as well as medium to high sand supply.
	// They are basically the default dune type obtained by any basic simulation scenario.
	std::cout << "Transverse dunes" << std::endl;
	DuneSediment dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 1.0, 3.0, Vector2(3, 0));
	for (int i = 0; i < 300; i++)
		dune.SimulationStepMultiThreadAtomic();
	dune.ExportObj("transverse.obj");
	std::cout << "Done 1/4" << std::endl << std::endl;

	// Barchan dunes appears under similar wind conditions, but lower sand supply.
	std::cout << "Barchan dunes" << std::endl;
	dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 0.5, 0.5, Vector2(3, 0));
	for (int i = 0; i < 300; i++)
		dune.SimulationStepMultiThreadAtomic();
	dune.ExportObj("barchan.obj");
	std::cout << "Done 2/4" << std::endl << std::endl;

	// Yardangs are created by abrasion, activated with a specific flag in our simulation.
	// Note: for more rounded yardangs, a turbulent wind is neccesary.
	std::cout << "Yardangs" << std::endl;
	dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 0.5, 0.5, Vector2(6, 0));
	dune.SetAbrasionMode(true);
	for (int i = 0; i < 600; i++)
		dune.SimulationStepMultiThreadAtomic();
	dune.ExportObj("yardangs.obj");
	std::cout << "Done 3/4" << std::endl << std::endl;

	// Nabkha are created under the influence of vegetation, also a flag to turn on.
	// Of course, it is quite difficult to see the differences withtout actually placing vegetation at the right places.
	// @Todo(Axel): Make a more interesting scene.
	std::cout << "Nabkha" << std::endl;
	dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 1.0, 3.0, Vector2(3, 0));
	dune.SetVegetationMode(true);
	for (int i = 0; i < 300; i++)
		dune.SimulationStepMultiThreadAtomic();
	dune.ExportObj("nabkha.obj");
	std::cout << "Done 4/4" << std::endl << std::endl;
}

/*!
\brief Running this program will export some
meshes similar to the ones seen in the paper.
*/
int main()
{
	ExportScenes();
	return 0;
}
