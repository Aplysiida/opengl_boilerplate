#pragma once
//std
#include <algorithm>
#include <fstream>
#include <sstream>
//project
#include "mesh.hpp"

Mesh::Mesh(const Mesh& obj) {
	//insert all vertices from obj to vertices
	vertices.insert(vertices.begin(), obj.vertices.begin(), obj.vertices.end());
	//insert all indices from obj to indices
	indices.insert(indices.begin(), obj.indices.begin(), obj.indices.end());
	exVertices.insert(exVertices.begin(), obj.exVertices.begin(), obj.exVertices.end()); //error here
	buildMesh();

	setupExVertices();
	copyNeighbours(obj);
}

Mesh::Mesh(const aiMesh* mesh, const aiScene* scene) {
	//vertices
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		//convert from assimp to glm
		//position
		glm::vec3 v;
		v.x = mesh->mVertices[i].x;
		v.y = mesh->mVertices[i].y;
		v.z = mesh->mVertices[i].z;
		vertex.pos = v;
		//normal
		if (mesh->HasNormals()) {
			v.x = mesh->mNormals[i].x;
			v.y = mesh->mNormals[i].y;
			v.z = mesh->mNormals[i].z;
			vertex.normal = v;
		}
		vertices.push_back(vertex);

		//don't need texture coord for now
	}
	//indices
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	buildMesh();
}

Mesh::Mesh(const std::vector<glm::vec3>& positions, const std::vector<unsigned int>& ind) {
	std::vector<glm::vec3> normals;
	for (auto i : ind) {
		indices.push_back(i);
	}

	generateNormals(positions, normals);
	for (int i = 0; i < positions.size(); i++) {
		Vertex vertex;
		vertex.pos = positions[i];
		vertex.normal = normals[i];
		vertices.push_back(vertex);
	}

	buildMesh();
	setupExVertices();
}

void Mesh::generateNormals(const std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals) {
	normals = std::vector< glm::vec3>(positions.size());
	for (int i = 0; i < indices.size(); i += 3) {
		//get face, assuming all faces are triangles because all provided models are triangles
		glm::vec3 a = positions[indices[i]];
		glm::vec3 b = positions[indices[i+1]];
		glm::vec3 c = positions[indices[i+2]];
		glm::vec3 normal = glm::normalize(glm::cross((b-a), (c-a)));	//compute face normal
		//assign face normals
		normals[indices[i]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
	}
	//normalize normals
	for (auto n : normals) {
		n = glm::normalize(n);
	}
}

void Mesh::draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

void Mesh::setupExVertices() {
	exVertices = std::vector<ExtendedVertex>();
	for (auto& v : vertices) {
		exVertices.push_back(ExtendedVertex(v));
	}
}

void Mesh::copyNeighbours(const Mesh& m) {
	for (int i = 0; i < exVertices.size(); i++) {
		exVertices[i].neighbouringVertices.insert(m.exVertices[i].neighbouringVertices.begin(),
												  m.exVertices[i].neighbouringVertices.end());
		//exVertices[i].neighbouringVertices.insert(exVertices[i].neighbouringVertices.begin(),
		//										  m.exVertices[i].neighbouringVertices.begin(),
		//										  m.exVertices[i].neighbouringVertices.end());
	}
}

void Mesh::setVertexNeighbours(std::vector<unsigned int> indices) {
	//for each vertex get neighbouring two vertices in face
	//for first vertex get final vertex as well
	exVertices[indices[0]].neighbouringVertices.insert(indices[1]);
	exVertices[indices[0]].neighbouringVertices.insert(indices[indices.size() -1]);
	//exVertices[indices[0]].neighbouringVertices.push_back(indices[1]);
	//exVertices[indices[0]].neighbouringVertices.push_back(indices[indices.size() - 1]);
	for (int i = 1; i < indices.size() - 1; i++) {
		exVertices[indices[i]].neighbouringVertices.insert(indices[i-1]);
		exVertices[indices[i]].neighbouringVertices.insert(indices[i+1]);
		//exVertices[indices[i]].neighbouringVertices.push_back(indices[i - 1]);
		//exVertices[indices[i]].neighbouringVertices.push_back(indices[i + 1]);
	}
	//for final vertex get first vertex as well
	exVertices[indices[indices.size() - 1]].neighbouringVertices.insert(indices[indices.size() - 2]);
	exVertices[indices[indices.size() - 1]].neighbouringVertices.insert(indices[0]);
	//exVertices[indices[indices.size() - 1]].neighbouringVertices.push_back(indices[indices.size() - 2]);
	//exVertices[indices[indices.size() - 1]].neighbouringVertices.push_back(indices[0]);
}

void Mesh::buildMesh() {
	//generate buffer objects/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	//vao
	glBindVertexArray(VAO);
	//vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	//ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//set up vertex attribute pointers
	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glBindVertexArray(0);
}