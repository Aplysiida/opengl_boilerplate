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
	//Mesh smoothing with uniform weights
	static Mesh applyLaplacian(const Mesh& m, float coreStrength, Eigen::SparseMatrix<double>& L);	
	//Minimal surface
	static Mesh minimalSurface(const Mesh& m, std::vector<unsigned int> anchors, Eigen::SparseMatrix<double>& L);
};