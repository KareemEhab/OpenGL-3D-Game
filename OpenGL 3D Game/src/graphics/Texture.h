#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/scene.h>

#include <stb/stb_image.h>

using namespace std;

class Texture
{
public:
	Texture();
	Texture(string dir, string path, aiTextureType type);

	void generate();
	void load(bool flip = true);

	void bind();

	unsigned int id;
	aiTextureType type;
	string dir;
	string path;
};

