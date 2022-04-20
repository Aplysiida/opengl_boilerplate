//eigen
#include <Eigen/Sparse>
//project
#include "boilerplate/mesh.hpp"

class MeshFairing {
private:
	//build the laplacian matrix with uniform weights, n = number of vertices
	static void buildLaplacianUniform(Mesh& mesh, Eigen::SparseMatrix<float>& l);
public:
	static Mesh core(const Mesh& m);	//Mesh fairing with uniform weights
};