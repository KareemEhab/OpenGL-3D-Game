#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../Model.h"

class Sphere : public Model
{
public:
	Sphere(unsigned int maxNoInstances)
		: Model("sphere", BoundTypes::SPHERE, maxNoInstances, NO_TEX | DYNAMIC) {}

	void init() {
		/*Texture tex("assets", "/image1.jpg", aiTextureType_DIFFUSE);
		tex.load();
		textures_loaded.push_back(tex);*/
		loadModel("assets/models/sphere/scene.gltf");
	}
};

#endif