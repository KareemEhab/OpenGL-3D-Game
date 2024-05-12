#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "Shader.h"
#include "GLMemory.hpp"

#include "../Scene.h"

using namespace std;

class Cubemap
{
public:
	Cubemap();

	void loadTextures(string dir,
		string right = "right.png",
		string left = "left.png",
		string top = "top.png",
		string bottom = "bottom.png",
		string front = "front.png",
		string back = "back.png");

	void init();

	void render(Shader shader, Scene* scene);

	void cleanup();

private:
	// Texture object
	unsigned int id;
	string dir;
	vector<string> faces;
	bool hasTextures;

	// Cube object
	ArrayObject VAO;
};

