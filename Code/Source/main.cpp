/*
	This is an example implementation of some of the results described in the paper "Desertscapes Simulation".
	No real time visualization in order to reduce dependencies. Running the program will output 3 .obj files which 
	can then be visualized in another application (Blender, MeshLab).

	If you have any questions, you can contact me at:
	axel(dot)paris(at)liris(dot)cnrs(dot)fr
*/

#include "desert.h"

/*!
\brief Running this program will export some
meshes similar to the ones seen in the paper.
*/
int main()
{
	DuneSediment dune = DuneSediment(Box2D(Vector2(0), Vector2(256)), 1.0, 5.0);
	for (int i = 0; i < 2; i++)
		dune.SimulationStepMultiThreadAtomic();
	return 0;
}
