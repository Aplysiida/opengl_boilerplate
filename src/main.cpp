//std
#include <iostream>
#include <sstream>

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
#include "scene.hpp"
#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//GLOBAL VAR    TODO: store these variables somewhere in a class
GLFWwindow* mWindow;
//window size
const int mWidth = 1280, mHeight = 960;
//shader location, remember program starts at build folder
std::string vertexShaderSource = "../res/shaders/vertex.glsl";
std::string fragmentShaderSource = "../res/shaders/fragment.glsl";
//default shader program used
GLSLShader shader;
//camera object
MoveAroundCamera mCamera;
OrbitalCamera oCamera;
Camera* camera; //camera pointer to currently selected camera
//mouse variables
float xLast = mWidth / 2;
float yLast = mHeight / 2;
bool leftPressed = false;    //left mouse button pressed 

Scene scene;

void render() {
    //render background colour
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //calc proj and view mat
    int width, height;
    glfwGetWindowSize(mWindow, &width, &height);
    glm::mat4 proj = glm::perspective(camera->fov, (float)width / (float)height, 0.1f, 2000.0f);
    glm::mat4 view = camera->getViewMat();
    //use shader
    shader.use();
    //set uniforms TODO:: put this in shader class maybe?
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uProjectionMatrix"), 1, false, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uModelViewMatrix"), 1, false, glm::value_ptr(view));

    scene.draw();
}

void renderGui(ImGuiIO& io) {
    //create new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //render gui
    //------ Insert Gui components HERE! -------
    ImGui::Begin("OpenGL boilerplate code");
    ImGui::Text("%.1f fps", io.Framerate);
    ImGui::Text("Use arrow keys to move MoveAround camera and scroll wheel\nto change Orbital camera distance");
    ImGui::Separator();
    ImGui::Text("Camera Options:");

    static int selectCamera = 0;
    ImGui::RadioButton("Move Around", &selectCamera, 0); ImGui::SameLine();
    ImGui::RadioButton("Orbital", &selectCamera, 1);

    ImGui::SliderFloat("Sensitivity", &camera->sensitivity, 0.01f, 1.0f);
    ImGui::SliderFloat("FOV", &camera->fov, 0.01f, 3.0f);

    switch (selectCamera) { 
        case 1: //orbital
            camera = &oCamera;
            ImGui::Text("Orbital Camera");
            ImGui::SliderFloat("Distance", &oCamera.distance, 0.0f, 800.0f);
            break;
        default:    //move around
            camera = &mCamera;
            ImGui::Text("Move Around Camera");
            ImGui::SliderFloat("Speed", &mCamera.cameraSpeed, 0.01f, 1.0f);
            break;
    }

    if (ImGui::Button("Reset camera position")) {
        mCamera.resetPos();
        oCamera.resetPos();
    }
    ImGui::Separator();
    ImGui::Text("Mesh");
    static char filename[1024] = "";
    if (ImGui::Button("Load Mesh")) {
        ImGui::OpenPopup("Load Mesh");
    }
    if (ImGui::BeginPopup("Load Mesh")) {
        ImGui::InputText("Filename", filename, 1024);
        if (ImGui::Button("Load")) {
            //load mesh here
            scene.destroy();
            scene = Scene(filename);
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
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        mCamera.moveCamera(Camera::Forward);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        mCamera.moveCamera(Camera::Left);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mCamera.moveCamera(Camera::Backward);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        mCamera.moveCamera(Camera::Right);
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        leftPressed = (action == GLFW_PRESS);       
    }
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
    //process mouse movement for camera only if left mouse button is currently pressed
    if (leftPressed) {
        camera->processMouseCamera(glm::vec2(xPos, yPos), glm::vec2(xLast, yLast));
    }

    xLast = xPos;
    yLast = yPos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    //process scroll movement for camera
    oCamera.distance -= (float)yoffset;
    if (oCamera.distance < 0.0f) {
        oCamera.distance = 0.0f;
    }
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
    mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGl", nullptr, nullptr);
    if (mWindow == nullptr) {
        std::cerr << "Failed to create window  " << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(mWindow, cursorPosCallback);
    glfwSetScrollCallback(mWindow, scrollCallback);

    //load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //build shader program to use
    shader = GLSLShader(vertexShaderSource, fragmentShaderSource);

    //initialise imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    //get current glsl version, 330 is recommended
    std::string str(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    //skip words to get to version number
    str = str.substr(0,4);
    str.erase(remove(str.begin(), str.end(), '.'));
    
    str.insert(0, "#version ");
    const char* glsl_version = str.c_str();


    //setup platform/renderer
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //setup style
    ImGui::StyleColorsClassic();

    camera = &mCamera;

    //render window loop
    while (!glfwWindowShouldClose(mWindow)) {
        processInput(mWindow);

        render();
        //render gui
        renderGui(io);

        // check and call events and swap the buffers
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    scene.destroy();
    //shutdown imgui and glfw
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}