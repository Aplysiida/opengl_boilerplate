//std
#include <vector>
//eigen
#include <Eigen/SparseQR>
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
		for (unsigned int j : exV.neighbouringVertices) {
			laplacian.insert(i, j) = -1.0/(double)diagonalValue;
		}
		laplacian.insert(i,i) = 1.0;
	}
	std::cout << laplacian << std::endl;
	laplacian.makeCompressed();
	l = laplacian;
}

void MeshFairing::buildLaplacianCotan(Mesh& mesh, Eigen::SparseMatrix<double>& l) {
	int n = mesh.getVerticesNum();
	//create empty nxn matrix, n = total number of vertices, should be dynamic
	Eigen::SparseMatrix<double> laplacian(n, n);
	//for each row/vertex
	for (int i = 0; i < n; i++) {
		ExtendedVertex exV = mesh.getExVertex(i);
		double sum = 0.0;	//sum of weights
		int diagonalValue = exV.neighbouringVertices.size();
		for (auto j : exV.neighbouringVertices) {
			ExtendedVertex exVj = mesh.getExVertex(j);	//extended vertex of neighbour j
			//get surrounding vertices
			std::vector<unsigned int> other;
			std::copy_if(exVj.neighbouringVertices.begin(), exVj.neighbouringVertices.end(), 
				std::back_inserter(other), [exV](unsigned int i) { return exV.neighbouringVertices.count(i); });
			//get angles
			//alpha
			glm::vec3 u = exV.getVertex().pos - mesh.getExVertex(other[0]).getVertex().pos;
			glm::vec3 v = exVj.getVertex().pos - mesh.getExVertex(other[0]).getVertex().pos;
			double cos = (glm::dot(u, v)) / (glm::length(u) * glm::length(v));
			double alpha = glm::acos(cos);
			//beta
			double beta;
			double weight;
			if (other.size() == 2) {
				u = exV.getVertex().pos - mesh.getExVertex(other[1]).getVertex().pos;
				v = exVj.getVertex().pos - mesh.getExVertex(other[1]).getVertex().pos;
				
				cos = (glm::dot(u, v)) / (glm::length(u) * glm::length(v));
				beta = glm::acos(cos);
				weight = (1.0 / glm::tan(alpha)) + (1.0 / glm::tan(beta));
			}
			else {	//if at edge like in hand.obj might only have one side to worry about
				weight = 1.0 / glm::tan(alpha);
			}
			weight *= 0.5;
			weight = (-1.0 / (double)diagonalValue) * weight;
			sum += weight;
			laplacian.insert(i, j) = weight;
		}
		laplacian.insert(i,i) = sum/(double)diagonalValue;
	}
	laplacian.makeCompressed();
	l = laplacian;
}

Mesh MeshFairing::applyLaplacian(const Mesh& m, float lamdba, Eigen::SparseMatrix<double>& L) {
	Mesh smoothMesh(m);
	int n = smoothMesh.getVerticesNum();

	Eigen::SparseMatrix<double> I(n, n);
	I.setIdentity();

	Eigen::SparseMatrix<double> newL = (lamdba * L) + I;
	std::vector<glm::vec3> positions(n);

	//for each vertex
	for (int i = 0; i < smoothMesh.getVerticesNum(); i++) {
		glm::vec3 moveAmount(0.0f);
		ExtendedVertex exV = smoothMesh.getExVertex(i);
		//for each neighbouring vertex
		for (auto j : exV.neighbouringVertices) { 
			glm::vec3 diff = (exV.getVertex().pos - smoothMesh.getExVertex(j).getVertex().pos);
			diff *=  newL.coeff(i, j);	//coeff timeconsuming, but works well enough
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

Mesh MeshFairing::minimalSurface(const Mesh& mesh, std::vector<unsigned int> anchors, Eigen::SparseMatrix<double>& L) {
	Mesh minMesh(mesh);
	int n = minMesh.getVerticesNum();
	int m = anchors.size();

	Eigen::VectorXd Vx(n);
	Eigen::VectorXd Vy(n);
	Eigen::VectorXd Vz(n);
	for (int i = 0; i < minMesh.getVerticesNum(); i++) {
		Vx[i] = minMesh.getExVertex(i).getVertex().pos.x;
		Vy[i] = minMesh.getExVertex(i).getVertex().pos.y;
		Vz[i] = minMesh.getExVertex(i).getVertex().pos.z;
	}

	//build matrix
	//A
	Eigen::SparseMatrix<double> A(n + m, n + m);
	//add L
	for (int i = 0; i < L.outerSize(); i++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(L,i); it; ++it) {
			A.insert(it.row(), it.col()) = it.value();
		}
	}
	for (int i = 0; i < m; i++) {
		int a = anchors[i];
		A.insert(a, n + i) = 1.0;
	}
	//add anchors
	for (int i = 0; i < m; i++) {
		int a = anchors[i];
		A.insert(n+i, a) = 1.0;
	}
	A.makeCompressed();

	//solve x
	Eigen::SparseVector<double> b(n + m);	//get b
	for (int i = 0; i < m; i++) {
		b.insert(n + i) = minMesh.getExVertex(anchors[i]).getVertex().pos.x;
	}
	Eigen::SparseQR<Eigen::SparseMatrix<double, Eigen::ColMajor, Eigen::Index>, Eigen::COLAMDOrdering<Eigen::Index> > solver;
	solver.compute(A);
	Vx = solver.solve(b);
	//solve y
	b = Eigen::SparseVector<double>(n + m);	//get b
	for (int i = 0; i < m; i++) {
		b.insert(n + i) = minMesh.getExVertex(anchors[i]).getVertex().pos.y;
	}
	solver.compute(A);
	Vy = solver.solve(b);
	//solve z
	b = Eigen::SparseVector<double>(n + m);	//get b
	for (int i = 0; i < m; i++) {
		b.insert(n + i) = minMesh.getExVertex(anchors[i]).getVertex().pos.z;
	}
	solver.compute(A);
	Vz = solver.solve(b);

	std::vector<glm::vec3> positions(n);
	for (int i = 0; i < minMesh.getVerticesNum(); i++) {
		glm::vec3 pos = glm::vec3(Vx[i], Vy[i], Vz[i]);
		positions.push_back(pos);
	}
	std::vector<unsigned int> indices = minMesh.getIndices();
	
	Mesh newMesh = Mesh(positions, indices);  //return new edited mesh
	newMesh.copyNeighbours(newMesh);
	return newMesh;
}