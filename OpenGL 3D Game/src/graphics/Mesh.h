#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
#include "GLMemory.hpp"

#include "models/Box.hpp"

#include "../algorithms/Bound.h"

using namespace std;

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
	ArrayObject VAO;

	vector<Texture> textures;
	aiColor4D diffuse;
	aiColor4D specular;

	Mesh(); // Default constructor 
	Mesh(BoundingRegion br, std::vector<Texture> textures = {}); // Textured object
	Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec); // Material object

	// load vertex and index data
	void loadData(std::vector<Vertex> vertices, vector<unsigned int> indices);

	void render(Shader shader, unsigned int noOfInstances);

	void cleanup();
	
private:
	unsigned int VBO, EBO;
	bool noTex;
	void setup();
};

