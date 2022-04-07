#pragma once
//std
#include <algorithm>
//project
#include "mesh.hpp"

void printVertex(Vertex v) {
	std::cout << "Position: {" << v.pos.x << ", " << v.pos.y << ", " << v.pos.z << "}" << std::endl;
}

Mesh::Mesh(const std::vector<Vertex>& v, const std::vector<unsigned int>& i) {
	vertices.insert(vertices.begin(), v.begin(), v.end());	//insert all vertices from v to vertices
	indices.insert(indices.begin(), i.begin(), i.end());	//insert all indices from i to indices
	//  
	//print vertices for now
	//for_each(v.begin(), v.end(), printVertex);
	buildMesh();
}

void Mesh::draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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