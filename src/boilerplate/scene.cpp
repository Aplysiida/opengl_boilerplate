//std
#include <fstream>
#include <sstream>
//project
#include "scene.hpp"

void Scene::processNode(aiNode* node, const aiScene* scene) {
    //process all meshes of node
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(Mesh(mesh, scene));
    }
    //do same for node's children
    for (int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Scene::loadScene(std::string filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_GenNormals);   //only generate normals
    //check that no error occured when loading scene
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR: " << importer.GetErrorString() << std::endl;
        throw SceneError();
    }

    //load mesh nodes
    processNode(scene->mRootNode, scene);
    std::cout << "Loaded scene" << std::endl;
}

void Scene::loadNeighbourVert(std::string filePath, Mesh& mesh) {
    //read f lines
    std::vector<std::string> fLines;
    std::ifstream in(filePath, std::ios::in);

    std::string line;
    while (std::getline(in, line)) {
        //tokenize line
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ' ');
        if (token == "f") { //at f line
            std::vector<unsigned int> indices;
            while (std::getline(ss, token, ' ')) {
                std::stringstream ssv(token); //get string stream for single vertex
                std::getline(ssv, token, '/');
                indices.push_back(stoi(token));
            }
            //assign neighbouring vertices
            mesh.setVertexNeighbours(indices);
        }
    }
}

Scene::Scene(std::string filePath) {
    try {
        loadScene(filePath);
    }
    catch (SceneError e) {
        std::cerr << "ERROR: scene cannot be loaded" << std::endl;
        return;
    }

    //do mesh fairing on mesh if there's only 1 mesh
    if (meshes.size() == 1) {
        //load neighbouring vertices info for mesh fairing
        meshes[0].setupExVertices();
        loadNeighbourVert(filePath, meshes[0]);
        //do core mesh smoothing
        meshes.push_back(MeshFairing::core(meshes[0]));
    }
}