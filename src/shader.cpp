#pragma once
#include "shader.hpp"



GLSLShader::GLSLShader(std::string vertexFile, std::string fragmentFile) {
    ID = build(vertexFile, fragmentFile);
}

void GLSLShader::use() {
    glUseProgram(ID);
}

/*
* Read the shader file into string
*/
std::string GLSLShader::readShaderFile(std::string filename) {
    std::ifstream fileStream(filename);
    if (!fileStream) {
        //std::cerr << "ERROR: Could not locate and open file " << filename << std::endl;
        throw std::runtime_error("ERROR: Could not locate and open file " + filename);
    }

    std::string shaderContent = ""; //stores entire shader
    std::string shaderLine = "";    //stores only a single line in shader
    while (!fileStream.eof()) {
        std::getline(fileStream, shaderLine);
        shaderContent += shaderLine + "\n";
    }
    fileStream.close();
    return shaderContent;
}

/*
* Check success of compiling shader
*/
bool GLSLShader::checkCompileSuccess(GLuint shader) {
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader compilation failed\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

/*
 * Builds shader program of vertex shader and fragment shader
 */
GLuint GLSLShader::build(std::string vertexFile, std::string fragmentFile) {
    //vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexShaderSource = "";
    try {
        vertexShaderSource = readShaderFile(vertexFile);
    }
    catch (std::runtime_error e) {
        std::cerr << "ERROR: cannot find vertex shader file" << std::endl;
        throw e;
    }
    const char* shaderStr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &shaderStr, NULL);
    glCompileShader(vertexShader);
    //check that compile succeeded
    if (!checkCompileSuccess(vertexShader)) {
        throw CompileError();
    }
    //fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentShaderSource = "";
    try {
        fragmentShaderSource = readShaderFile(fragmentFile);
    }
    catch (std::runtime_error e) {
        std::cerr << "ERROR: cannot find fragment shader file" << std::endl;
        throw e;
    }
    shaderStr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &shaderStr, NULL);
    glCompileShader(fragmentShader);
    //check that compile succeeded
    if (!checkCompileSuccess(fragmentShader)) {
        throw CompileError();
    }

    //link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //check that link succeeded
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        throw LinkError();
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}