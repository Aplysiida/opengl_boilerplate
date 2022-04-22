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
    std::vector<Mesh> meshes;
    std::vector<glm::vec3> anchorPoints;    //used for completion

    void processNode(aiNode* node, const aiScene* scene);
    void loadScene(std::string filePath, bool useAssimp);
    void loadNeighbourVert(std::string filePath, Mesh& mesh);
public:
    int currentMesh = 0;    //current mesh to draw

    Scene() {}
    Scene(std::string filePath);
    
    void doCore(float coreStrength, int coreIter) {
        std::cout << "Recalculating core" << std::endl;
        meshes[1] = MeshFairing::core(meshes[0], coreStrength, uniformLaplacian);
        for (int i = 0; i < coreIter; i++) {
            meshes[1] = MeshFairing::core(meshes[1], coreStrength, uniformLaplacian);
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