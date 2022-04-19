//project
#include "boilerplate/mesh.hpp"

class MeshFairing {
private:
	static void buildLaplacian(Mesh& mesh);	//build the laplacian matrix, n = number of vertices
public:
	static Mesh core(const Mesh& m);	//Mesh fairing with uniform weights
};