#include "Light.h"

void DirLight::render(Shader shader)
{
	string name = "dirLight";

	shader.set3Float(name + ".direction", direction);
	shader.set3Float(name + ".ambient", ambient);
	shader.set3Float(name + ".diffuse", diffuse);
	shader.set3Float(name + ".specular", specular);
}

void PointLight::render(Shader shader, int idx)
{
	string name = "pointLights[" + to_string(idx) + ']';

	shader.set3Float(name + ".position", position);
	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);

	shader.set3Float(name + ".ambient", ambient);
	shader.set3Float(name + ".diffuse", diffuse);
	shader.set3Float(name + ".specular", specular);
}

void SpotLight::render(Shader shader, int idx)
{
	string name = "spotLights[" + to_string(idx) + ']';

	shader.set3Float(name + ".position", position);
	shader.set3Float(name + ".direction", direction);
	shader.setFloat(name + ".cutOff", cutOff);
	shader.setFloat(name + ".outerCutOff", outerCutOff);
	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);

	shader.set3Float(name + ".ambient", ambient);
	shader.set3Float(name + ".diffuse", diffuse);
	shader.set3Float(name + ".specular", specular);
}