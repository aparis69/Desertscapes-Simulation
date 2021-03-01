/*
	This is an example implementation of some of the results described in the paper "Desertscapes Simulation".

	No real time visualization in order to reduce dependencies. Running the program will output 3 .obj files which
	can then be visualized in another application (Blender, MeshLab).

	If you have any questions, you can contact me at:
	axel(dot)paris(at)liris(dot)cnrs(dot)fr
*/

#include "desert.h"
#include <chrono>

void OptimizedScene()
{
	// This function is the optimized version of the simulation, without slow atomic operations
	// That I used in the original. This version is up to 4x faster, and is also can be easily
	// Implemented in a Compute shader for even faster speedup ;-)
	DuneSediment dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 1.0, 3.0, Vector2(1, 0));
	auto start = std::chrono::steady_clock::now();
	{
		for (int i = 0; i < 100; i++)
			dune.StepNoAtomic();
	}
	auto end = std::chrono::steady_clock::now();
	std::cout << "Elapsed time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
	dune.ExportObj("transverse.obj");
}

/*!
\brief
*/
void ExportScenes()
{
	// Transverse dunes are created under unimodal wind, as well as medium to high sand supply.
	// They are basically the default dune type obtained by any basic simulation scenario.
	std::cout << "Transverse dunes" << std::endl;
	DuneSediment dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 1.0, 3.0, Vector2(1, 0));
	auto start = std::chrono::steady_clock::now();
	{
		for (int i = 0; i < 100; i++)
			dune.SimulationStepMultiThreadAtomic();
	}
	auto end = std::chrono::steady_clock::now();
	std::cout << "Elapsed time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
	dune.ExportObj("transverse.obj");
	std::cout << "Done 1/4" << std::endl << std::endl;

	//// Barchan dunes appears under similar wind conditions, but lower sand supply.
	//std::cout << "Barchan dunes" << std::endl;
	//dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 0.5, 0.5, Vector2(3, 0));
	//for (int i = 0; i < 300; i++)
	//	dune.SimulationStepMultiThreadAtomic();
	//dune.ExportObj("barchan.obj");
	//std::cout << "Done 2/4" << std::endl << std::endl;

	//// Yardangs are created by abrasion, activated with a specific flag in our simulation.
	//// Note: for more rounded yardangs, a turbulent wind is neccesary.
	//std::cout << "Yardangs" << std::endl;
	//dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 0.5, 0.5, Vector2(6, 0));
	//dune.SetAbrasionMode(true);
	//for (int i = 0; i < 600; i++)
	//	dune.SimulationStepMultiThreadAtomic();
	//dune.ExportObj("yardangs.obj");
	//std::cout << "Done 3/4" << std::endl << std::endl;

	//// Nabkha are created under the influence of vegetation, also a flag to turn on.
	//// Of course, it is quite difficult to see the differences withtout actually placing vegetation at the right places.
	//// @Todo(Axel): Make a more interesting scene.
	//std::cout << "Nabkha" << std::endl;
	//dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 1.0, 3.0, Vector2(3, 0));
	//dune.SetVegetationMode(true);
	//for (int i = 0; i < 300; i++)
	//	dune.SimulationStepMultiThreadAtomic();
	//dune.ExportObj("nabkha.obj");
	//std::cout << "Done 4/4" << std::endl << std::endl;
}

/*!
\brief Running this program will export some
meshes similar to the ones seen in the paper.
*/
int main()
{
	ExportScenes();
	//OptimizedScene();
	return 0;
}
