#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Shader
{
public:
	unsigned int id;

	Shader();
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	
	void generate(const char* vertexShaderPath, const char* fragShaderPath);
	void activate();

	// Utility functions
	string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);

	// Uniform functions
	void setMat4(const string& name, glm::mat4 val) { glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val)); }
    void setBool(const std::string& name, bool value) { glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value); }
    void setInt(const string& name, int value) { glUniform1i(glGetUniformLocation(id, name.c_str()), value); }
    void setFloat(const std::string& name, float value){ glUniform1f(glGetUniformLocation(id, name.c_str()), value); }
	void set3Float(const std::string& name, glm::vec3 value) { set3Float(name, value.x, value.y, value.z); }
	void set3Float(const std::string& name, float x, float y, float z) { glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z); }
};

