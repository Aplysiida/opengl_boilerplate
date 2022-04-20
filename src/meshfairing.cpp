//std
#include <vector>
//project
#include "meshfairing.hpp"

void MeshFairing::buildLaplacianUniform(Mesh& mesh, Eigen::SparseMatrix<float>& l) {
	int n = mesh.getVerticesNum();
	//create empty nxn matrix, n = total number of vertices, should be dynamic
	Eigen::SparseMatrix<float> laplacian(n, n);
	std::vector<Eigen::Triplet<float>> triplets;
	//for each row/vertex
	for (int i = 0; i < n; i++) {
		//find neighbouring vertices
		ExtendedVertex exV = mesh.getExVertex(i);
		int diagonalValue = 0;
		for (unsigned int index : exV.neighbouringVertices) {
			//set corresponding elements in row to neighbouring vertices
			triplets.push_back(Eigen::Triplet<float>(index,i,1));
			diagonalValue++;
		}
		triplets.push_back(Eigen::Triplet<float>(i,i,diagonalValue));	//set diagonal element
	}
	laplacian.setFromTriplets(triplets.begin(), triplets.end());	//add triplets to matrix
	laplacian.makeCompressed();
	l = laplacian;
}

Mesh MeshFairing::core(const Mesh& m) {
	Mesh smoothMesh(m);
	smoothMesh.setupExVertices();
	int n = smoothMesh.getVerticesNum();
	Eigen::SparseMatrix<float> L(n, n);
	buildLaplacianUniform(smoothMesh, L);	//build sparse laplacian matrix
	Eigen::VectorXf Vx(n), Vy(n), Vz(n);	//get input vertices
	for (int i = 0; i < smoothMesh.getVerticesNum(); i++) {
		Vx[i] = (smoothMesh.getExVertex(i).vertex.pos.x);
		Vy[i] = (smoothMesh.getExVertex(i).vertex.pos.y);
		Vz[i] = (smoothMesh.getExVertex(i).vertex.pos.z);
	}
	Vx = L * Vx;
	Vy = L * Vy;
	Vz = L * Vz;

	//generate vertices
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	for (int i = 0; i < smoothMesh.getVerticesNum(); i++) {
		positions.push_back(glm::vec3(Vx[i], Vy[i], Vz[i]));
	}
	smoothMesh.generateNormals(positions, normals);
	std::vector<Vertex> vertices;
	for (int i = 0; i < smoothMesh.getVerticesNum(); i++) {
		Vertex v;// (positions[i], normals[i]);
		v.pos = positions[i];
		v.normal = normals[i];
		vertices.push_back(v);
	}
	std::vector<unsigned int> indices = smoothMesh.getIndices();
	return Mesh(vertices, indices);

	//return smoothMesh;	//return new edited mesh to draw
}