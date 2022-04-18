//project
#include "boilerplate/mesh.hpp"

class MeshFairing {
private:
	Mesh inputMesh;

	void buildLaplacian();
public:
	MeshFairing(const Mesh& inputM) : inputMesh(inputM) {}

	//Mesh core();	//Mesh fairing with uniform weights
};