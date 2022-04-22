//eigen
#include <Eigen/Sparse>
//project
#include "boilerplate/mesh.hpp"

class MeshFairing {
private:
public:
	//build the laplacian matrix with uniform weights, n = number of vertices
	static void buildLaplacianUniform(Mesh& mesh, Eigen::SparseMatrix<double>& l);
	//build the laplacian matrix with cotan weights, n = number of vertices
	static void buildLaplacianCotan(Mesh& mesh, Eigen::SparseMatrix<double>& l);
	//Mesh fairing with uniform weights
	static Mesh core(Mesh& m, float coreStrength, Eigen::SparseMatrix<double>& L);	
};