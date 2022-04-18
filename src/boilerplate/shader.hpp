#pragma once
//std
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
//glad
#include <glad/glad.h>

//structs used for error handling
struct ShaderError : public std::runtime_error {
public:
    explicit ShaderError(const std::string& what_ = "Shader failed.") : std::runtime_error(what_) { }
};

struct CompileError : public ShaderError {
public:
    explicit CompileError(const std::string& what_ = "Shader compilation failed.") : ShaderError(what_) { }
};

struct LinkError : public ShaderError {
public:
    explicit LinkError(const std::string& what_ = "Shader link failed.") : ShaderError(what_) { }
};

class GLSLShader {
private:
    std::string readShaderFile(std::string filename);
    bool checkCompileSuccess(GLuint shader);
    GLuint build(std::string vertexFile, std::string fragmentFile);
public:
    unsigned int ID;
    GLSLShader() { ID = NULL; } //create empty shader
    GLSLShader(std::string vertexFile, std::string fragmentFile);
    void use();
};