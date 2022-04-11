//std
#include <iostream>

//imgui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h>

//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//project
#include "camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//GLOBAL VAR    TODO: store these variables somewhere in a class
//window size
const int mWidth = 600, mHeight = 800;
//shader location, remember program starts at build folder
std::string vertexShaderSource = "../res/shaders/vertex.glsl";
std::string fragmentShaderSource = "../res/shaders/fragment.glsl";
//default shader program used
GLSLShader shader;
//camera object
MoveAroundCamera mCamera;
//mouse variables
float xLast = mWidth / 2;
float yLast = mHeight / 2;

void render(Mesh m) {
    //render background colour
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //calc proj and view mat
    //glm::mat4 proj = glm::perspective(1.0f, (float)mWidth/(float)mHeight, 0.1f, 1000.0f);
    glm::mat4 proj = glm::mat4(1.0);
    //glm::mat4 view = mCamera.getViewMat();
    glm::mat4 view = glm::mat4(1.0);
    //use shader
    shader.use();
    //set uniforms TODO:: put this in shader class maybe?
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uProjectionMatrix"), 1, false, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uModelViewMatrix"), 1, false, glm::value_ptr(view));

    m.draw();

}

void renderGui(ImGuiIO& io) {
    //create new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //render gui
    //------ Insert Gui components HERE! -------
    ImGui::Begin("OpenGL boilerplate code");
    static char filename[1024] = "";
    if (ImGui::Button("Load Mesh")) {
        ImGui::OpenPopup("Load Mesh");
    }
    if (ImGui::BeginPopup("Load Mesh")) {
        ImGui::InputText("Filename", filename, 1024);
        if (ImGui::Button("Load")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::End();

    //render imgui onto screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


/*
* Process all input: query GLFW for keys pressed
*/
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    //calculate mouse movement offset
    float xOffset = xPos - xLast;
    float yOffset = yPos - yLast;
    xLast = xPos;
    yLast = yPos;
    //might add sensitivity later, add it to imgui as well

    //process mouse movement for camera only if left mouse button is currently pressed
    mCamera.processMouseCamera(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    //process scroll movement for camera
}

/*
* Called when window size is changed
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ------ MAIN FUNCTION -------

int main() {
    //load glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //create glfw window
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGl", nullptr, nullptr);
    if (mWindow == nullptr) {
        std::cerr << "Failed to create window  " << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(mWindow, mouseCallback);
    glfwSetScrollCallback(mWindow, scrollCallback);

    //load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //build shader program to use
    shader = GLSLShader(vertexShaderSource, fragmentShaderSource);

    //initialise imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    //todo: change this to get current glsl version
    //std::cout << "glsl = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    //"version " + 
    const char* glsl_version = "#version 330";

    //setup platform/renderer
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //setup style
    ImGui::StyleColorsClassic();


    //create triangle
    Vertex v[] = {
        Vertex{glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0,0,1)},
        Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0,0,1)},
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0,0,1)},
        Vertex{glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0,0,1)}
    };
    unsigned int i[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    std::vector<Vertex> ver(std::begin(v), std::end(v));
    std::vector<unsigned int> ind(std::begin(i), std::end(i));

    Mesh m(ver, ind);

    //render window loop
    while (!glfwWindowShouldClose(mWindow)) {
        processInput(mWindow);

        render(m);
        //render gui
        renderGui(io);

        // check and call events and swap the buffers
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    m.destroy();
    //shutdown imgui and glfw
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}