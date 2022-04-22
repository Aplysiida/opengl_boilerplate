//std
#include <vector>
//project
#include "meshfairing.hpp"

void MeshFairing::buildLaplacianUniform(Mesh& mesh, Eigen::SparseMatrix<double>& l) {
	int n = mesh.getVerticesNum();
	//create empty nxn matrix, n = total number of vertices, should be dynamic
	Eigen::SparseMatrix<double> laplacian(n, n);
	//for each row/vertex
	for (int i = 0; i < n; i++) {
		//find neighbouring vertices
		ExtendedVertex exV = mesh.getExVertex(i);
		int diagonalValue = exV.neighbouringVertices.size();
		for (unsigned int index : exV.neighbouringVertices) {
			laplacian.insert(i, index) = -1.0/(double)diagonalValue;
		}
		laplacian.insert(i,i) = 1.0;
	}
	laplacian.makeCompressed();
	l = laplacian;
}

Mesh MeshFairing::core(Mesh& m, float coreStrength, Eigen::SparseMatrix<double>& L) {
	Mesh smoothMesh(m);
	int n = smoothMesh.getVerticesNum();

	Eigen::SparseMatrix<double> I(n, n);
	I.setIdentity();

	Eigen::SparseMatrix<double> newL = (coreStrength * L) + I;
	std::vector<glm::vec3> positions(n);

	//for each vertex
	for (int i = 0; i < smoothMesh.getVerticesNum(); i++) {
		glm::vec3 moveAmount(0.0f);
		ExtendedVertex exV = smoothMesh.getExVertex(i);
		//for each neighbouring vertex
		for (auto j : exV.neighbouringVertices) {
			glm::vec3 diff = (exV.getVertex().pos - smoothMesh.getExVertex(j).getVertex().pos);
			diff *= coreStrength * newL.coeff(i, j);	//coeff timeconsuming, but works well enough
			moveAmount += diff;
		}
		//calculate difference
		positions[i] = (exV.getVertex().pos + moveAmount);
	}
		 
	std::vector<unsigned int> indices = smoothMesh.getIndices();
	Mesh newMesh = Mesh(positions, indices);  //return new edited mesh
	newMesh.copyNeighbours(smoothMesh);
	return newMesh;
}