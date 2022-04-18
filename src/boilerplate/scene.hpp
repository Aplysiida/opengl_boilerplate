//assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
//project
#include "mesh.hpp"

//error structs
struct SceneError : public std::runtime_error {
public:
    explicit SceneError(const std::string& what_ = "Scene Loading failed.") : std::runtime_error(what_) { }
};

class Scene {
private:
    std::vector<Mesh> meshes;

    void processNode(aiNode* node, const aiScene* scene) {
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

    void loadScene(std::string filePath) {
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
public:
    Scene() {}
    Scene(std::string filePath) {
        try {
            loadScene(filePath);
        }
        catch (SceneError e) {
            std::cerr << "ERROR: scene cannot be loaded" << std::endl;
        }
    }

    void draw() {
        for (auto mesh : meshes) {
            mesh.draw();
        }
    }

    void destroy() {
        for (auto mesh : meshes) {
            mesh.destroy();
        }
        meshes = std::vector<Mesh>();
    }
};


