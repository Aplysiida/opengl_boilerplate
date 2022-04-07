#pragma once
//std
#include <iostream>
#include <vector>
//glm
#include <glm/glm.hpp>
//glad
#include <glad/glad.h>

/*
* Represents a single vertex
*/
struct Vertex {	//don't need texture coord for now
	//remember order of vertices matters since reading it into gpu mem
	glm::vec3 pos{ 0.0f };
	glm::vec3 normal{ 0.0f };
};

/*
* Represents the mesh structure
*/
class Mesh {	//don't need texture for now
private:
	//actual data of mesh
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	GLuint VBO;
	GLuint VAO;
	GLuint IBO;

	void buildMesh();
public:
	Mesh(const std::vector<Vertex>& v, const std::vector<unsigned int>& i);
	void draw();
	//void destroy();
};