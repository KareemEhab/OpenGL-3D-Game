#ifndef LAMP_HPP
#define LAMP_HPP

#include "Cube.hpp"
#include "../Light.h"

class Lamp : public Cube
{
public:
	glm::vec3 lightColor;

	PointLight pointLight;

	Lamp() {}

	Lamp(glm::vec3 lightColor,
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		float k0,
		float k1,
		float k2,
		glm::vec3 pos,
		glm::vec3 size)
		: lightColor(lightColor),
		pointLight({ pos, k0, k1, k2, ambient, diffuse, specular }),
		Cube(pos, size){ }

	// Override
	void render(Shader shader, float dt)
	{
		// Set light color
		shader.set3Float("lightColor", lightColor);

		Cube::render(shader, dt);
	}
};

#endif