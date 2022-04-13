#pragma once
//std
#include <iostream>
#include <vector>
//assimp
#include <assimp/scene.h>
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
	Mesh(const aiMesh* mesh, const aiScene* scene);	//build mesh if have mesh node from assimp
	Mesh(const std::vector<Vertex>& v, const std::vector<unsigned int>& i);	//build mesh if already have vertices and indices defined
	void draw();
	void destroy();
};