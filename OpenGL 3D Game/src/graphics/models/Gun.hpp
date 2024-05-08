#include "../Model.h"
#include "../../io/Camera.h"
#include "../../io/Keyboard.h"
#include "../../Scene.h"

class Gun : public Model
{
public:
	Gun(unsigned int maxNoInstances)
		: Model("m4a1", BoundTypes::AABB, maxNoInstances, CONST_INSTANCES | NO_TEX) { }

	void render(Shader shader, float dt, Scene *scene, bool setModel = false)
	{
		// Set position
		glm::mat4 model = glm::mat4(1.0f);
		rb.pos = scene->getActiveCamera()->getPos() 
			+ glm::vec3(scene->getActiveCamera()->getFront() * 0.25f) 
			+ glm::vec3(scene->getActiveCamera()->getUp() * -0.08f) 
			+ glm::vec3(scene->getActiveCamera()->getRight() * 0.085f);
		model = glm::translate(model, rb.pos);

		float theta;

		// Rotate around cameraRight using dot product
		theta = acos(
			glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), scene->getActiveCamera()->getFront())
			/ glm::length(scene->getActiveCamera()->getUp())
			/ glm::length(scene->getActiveCamera()->getFront())
		);
		model = glm::rotate(model, glm::radians(90.0f) - theta, scene->getActiveCamera()->getRight()); // Offset by pi/2 radians bc angle between camFront and gunFront

		// Rotate around cameraUp using dot product
		glm::vec2 front2d = glm::vec2(scene->getActiveCamera()->getFront().x, scene->getActiveCamera()->getFront().z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), front2d) / glm::length(front2d));
		model = glm::rotate(model, scene->getActiveCamera()->getFront().z < 0 ? theta : -theta, scene->getActiveCamera()->getWorldUp());

		model = glm::scale(model, size);
		shader.setMat4("model", model);

		Model::render(shader, dt, scene, false); // We pass false for setModel because we set it on the line before that
	}
};