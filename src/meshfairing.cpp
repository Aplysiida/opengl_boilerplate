//eigen
#include <Eigen/Sparse>
//std
#include <vector>
//project
#include "meshfairing.hpp"

void MeshFairing::buildLaplacian(Mesh& mesh) {
	int n = mesh.getVerticesNum();
	//create empty nxn matrix, n = total number of vertices, should be dynamic
	Eigen::SparseMatrix<double> laplacian(n,n);
	std::vector<Eigen::Triplet<double>> triplets;
	//for each row/vertex
	for (int i = 0; i < n; i++) {
		//find neighbouring vertices
		//set corresponding elements in row to neighbouring vertices
		//set diagonal element
	}
	//add triplets to matrix
	laplacian.setFromTriplets(triplets.begin(), triplets.end());
	laplacian.makeCompressed();
}

Mesh MeshFairing::core(const Mesh& m) { //change return type to mesh later
	Mesh smoothMesh(m);
	smoothMesh.printI();
	return smoothMesh;
	//build sparse laplacian matrix
	//for each vertex in mesh
	//solve Lx = 0
	//result is new vertex?
	//return new edited mesh to draw
}