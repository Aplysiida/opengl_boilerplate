//assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
//project
#include "mesh.hpp"
#include "meshfairing.hpp"

//error structs
struct SceneError : public std::runtime_error {
public:
    explicit SceneError(const std::string& what_ = "Scene Loading failed.") : std::runtime_error(what_) { }
};

class Scene {
private:
    //variables for assignment
    Eigen::SparseMatrix<double> uniformLaplacian;
    Eigen::SparseMatrix<double> cotanLaplacian;
    std::vector<Mesh> meshes;
    std::vector<unsigned int> anchorIndices;    //used for completion

    void processNode(aiNode* node, const aiScene* scene);
    void loadScene(std::string filePath, bool useAssimp);
    void loadNeighbourVert(std::string filePath, Mesh& mesh);
public:
    int currentMesh = 0;    //current mesh to draw

    Scene() {}
    Scene(std::string filePath);
    void setAnchorPoints(std::string filePath); //for completion
    
    void doCore(float lambda, int iter) {
        std::cout << "Recalculating core" << std::endl;
        meshes[1] = MeshFairing::applyLaplacian(meshes[0], lambda, uniformLaplacian);
        for (int i = 0; i < iter; i++) {
            meshes[1] = MeshFairing::applyLaplacian(meshes[1], lambda, uniformLaplacian);
        }
        std::cout << "Finished" << std::endl;
    }

    void doChallengeCotan(float lambda, int iter) {
        std::cout << "Recalculating challenge cotan" << std::endl;
        meshes[3] = MeshFairing::applyLaplacian(meshes[0], lambda, cotanLaplacian);
        for (int i = 0; i < iter; i++) {
            meshes[3] = MeshFairing::applyLaplacian(meshes[3], lambda, cotanLaplacian);
        }
        std::cout << "Finished" << std::endl;
    }

    //used for this assignment since only want to draw one mesh
    void drawMesh() {
        if (!meshes.empty()) {
            meshes[currentMesh].draw();
        }
    }

    //don't need to draw all meshes in scene so don't use this function
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