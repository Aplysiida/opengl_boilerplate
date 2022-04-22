#pragma once
//std
#include <iostream>
#include <set>
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
* Represents vertex that has been extended to include neighbouring vertices
*/
class ExtendedVertex {
private:
	Vertex vertex;

public:
	std::set<unsigned int> neighbouringVertices;	//index of neighbouring vertices
	ExtendedVertex() {}
	ExtendedVertex(Vertex v) : vertex(v) {}
	ExtendedVertex(const ExtendedVertex& obj) { 
		vertex = obj.vertex; 
		neighbouringVertices.insert(obj.neighbouringVertices.begin(), obj.neighbouringVertices.end());
	}

	Vertex& getVertex() { return vertex; }
};

/*
* Represents the mesh structure
*/
class Mesh {	//don't need texture for now
private:
	//actual data of mesh
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	//for mesh fairing
	std::vector<ExtendedVertex> exVertices;

	GLuint VBO;
	GLuint VAO;
	GLuint IBO;

	void buildMesh();
public:
	Mesh() { VAO = -1; VBO = -1; IBO = -1; }
	Mesh(const Mesh& obj);	//copy constructor
	Mesh(const aiMesh* mesh, const aiScene* scene);	//build mesh if have mesh node from assimp
	Mesh(const std::vector<glm::vec3>& positions, const std::vector<unsigned int>& i);
	void generateNormals(const std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals);
	void draw();
	void destroy();

	ExtendedVertex getExVertex(int i) { return exVertices[i]; }
	void setupExVertices();	//set up extended vertices
	void copyNeighbours(const Mesh& m);
	void setVertexNeighbours(std::vector<unsigned int> indices);	//set up vertex neighbours
	int getVerticesNum() { return vertices.size(); }

	std::vector<unsigned int> getIndices() { return indices; }
};