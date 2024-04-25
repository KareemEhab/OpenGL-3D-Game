#pragma once

#include <glm/glm.hpp>
#include "Shader.h"

struct DirLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader);
};

struct PointLight {
	glm::vec3 position;

	// Attenuation constants
	float k0;
	float k1;
	float k2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;			// Field of view of the light
	float outerCutOff;		// Field of view of the light for fading out
	
	// Attenuation constants
	float k0;
	float k1;
	float k2;
	
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

class Light
{
};

