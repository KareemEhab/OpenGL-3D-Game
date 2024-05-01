#include "../model.h"
#include "ModelArray.hpp"

class Sphere : public Model
{
public:
	Sphere(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f))
	: Model(pos, size, true) {}

	void init()
	{
		loadModel("assets/models/sphere/scene.gltf");
	}
};


class SphereArray : public ModelArray<Sphere>
{
public:
	void init()
	{
		model = Sphere(glm::vec3(0.0f), glm::vec3(0.1f));
		model.init();
	}
};