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

void Scene::loadScene(std::string filePath, bool useAssimp) {
    if (useAssimp) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath, aiProcess_JoinIdenticalVertices);   //only generate normals
        //check that no error occured when loading scene
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR: " << importer.GetErrorString() << std::endl;
            throw SceneError();
        }

        //load mesh nodes
        processNode(scene->mRootNode, scene);
    }
    else {  //not using assimp for this project so just loading 1 obj file
        std::vector<std::string> fLines;
        std::ifstream in(filePath, std::ios::in);

        std::vector<glm::vec3> positions;
        std::vector<unsigned int> indices;

        std::string line;
        while (std::getline(in, line)) {
            //tokenize line
            std::stringstream ss(line);
            std::string token;
            std::getline(ss, token, ' ');
            //for vertices
            if (token == "v") { //for positions and normals
                std::getline(ss, token, ' ');
                float x = stof(token);
                std::getline(ss, token, ' ');
                float y = stof(token);
                std::getline(ss, token, ' ');
                float z = stof(token);
                positions.push_back(glm::vec3(x, y, z));
            }
            else if (token == "f") {	//for faces
                while (std::getline(ss, token, ' ')) {
                    std::stringstream ssv(token); //get string stream for single vertex
                    std::getline(ssv, token, '/'); 
                    indices.push_back(stoi(token)-1);
                }
            }
        }
        Mesh mesh = Mesh(positions, indices);
        meshes.push_back(mesh);
    }
    std::cout << "Loaded scene" << std::endl;
}

void Scene::loadNeighbourVert(std::string filePath, Mesh& mesh) {
    //read f lines
    std::vector<std::string> fLines;
    std::ifstream in(filePath, std::ios::in);

    std::string line;
    int faceNum = 0;
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
                indices.push_back(stoi(token)-1);
            }
            //assign neighbouring vertices
            mesh.setVertexNeighbours(indices);
            faceNum++;
        }
    }
}

Scene::Scene(std::string filePath) {
    try {
        loadScene(filePath, false);
    }
    catch (SceneError e) {
        std::cerr << "ERROR: scene cannot be loaded" << std::endl;
        return;
    }

    //do assignment stuff on mesh if there's only 1 mesh
    if (meshes.size() == 1) {
        //load neighbouring vertices info for mesh fairing
        meshes[0].setupExVertices();
        std::cout << "calculating neighbours" << std::endl;
        loadNeighbourVert(filePath, meshes[0]);
        std::cout << "building laplacians" << std::endl;
        MeshFairing::buildLaplacianUniform(meshes[0], uniformLaplacian);
        MeshFairing::buildLaplacianCotan(meshes[0], cotanLaplacian);
        //apply smoothing
        std::cout << "applying mesh smoothing" << std::endl;
        meshes.push_back(MeshFairing::applyLaplacian(meshes[0], 0.5, uniformLaplacian));
        meshes.push_back(meshes[0]);    //add completion mesh for later
        meshes.push_back(MeshFairing::applyLaplacian(meshes[0], 0.5, cotanLaplacian));
        std::cout << "finished smoothing" << std::endl;
    }
}

void Scene::setAnchorPoints(std::string filePath) {
    if(meshes.empty()) {    //don't bother
        std::cout << "ERROR: scene not loaded, load scene first" << std::endl;
        return;
    }
    std::ifstream in(filePath, std::ios::in);
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ' ');
        if (token == "i") {
            std::getline(ss, token, ' ');
            anchorIndices.push_back(stoi(token));
        }
    }
    //since anchor and mesh now exist, create core mesh
    std::cout << "applying minimal surface" << std::endl;
    meshes[2] = MeshFairing::minimalSurface(meshes[0], anchorIndices, uniformLaplacian);
    std::cout << "finished" << std::endl;
}