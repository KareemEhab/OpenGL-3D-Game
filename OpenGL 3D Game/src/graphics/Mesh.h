#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	static vector<struct Vertex> genList(float* vertices, int noVertices);
};
typedef struct Vertex Vertex;

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indeces;
	unsigned int VAO;

	vector<Texture> textures;

	Mesh();
	Mesh(vector<Vertex> vertices, vector<unsigned int> indeces, vector<Texture> textures);

	void render(Shader shader);

	void cleanup();
	
private:
	unsigned int VBO, EBO;
	void setup();
};

