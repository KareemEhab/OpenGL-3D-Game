#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

#include "models/Box.hpp"

#include "../algorithms/Bound.h"

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
	BoundingRegion br;

	vector<Vertex> vertices;
	vector<unsigned int> indeces;
	unsigned int VAO;

	vector<Texture> textures;
	aiColor4D diffuse;
	aiColor4D specular;

	Mesh(BoundingRegion br, vector<Vertex> vertices, vector<unsigned int> indeces, vector<Texture> textures = {});
	Mesh(BoundingRegion br, vector<Vertex> vertices, vector<unsigned int> indeces, aiColor4D diffuse, aiColor4D specular);

	void render(Shader shader, glm::vec3 pos, glm::vec3 size, Box *box, bool doRender = true);

	void cleanup();
	
private:
	unsigned int VBO, EBO;
	bool noTex;
	void setup();
};

