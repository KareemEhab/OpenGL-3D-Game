#ifndef LAMP_HPP
#define LAMP_HPP

#include "Cube.hpp"
#include "../Light.h"
#include "ModelArray.hpp"

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

class LampArray : public ModelArray<Lamp>
{
public:
	vector<PointLight> lightInstances;

	void init()
	{
		model = Lamp(glm::vec3(1.0f),
			glm::vec4(0.05f, 0.05f, 0.05f, 1.0f),
			glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			1.0f, 0.07f, 0.032f,
			glm::vec3(0.0f), glm::vec3(0.25f));
		model.init();
	}

	void render(Shader shader, float dt)
	{
		for (PointLight pl : lightInstances)
		{
			model.rb.pos = pl.position;
			model.render(shader, dt);
		}
	}
};

#endif