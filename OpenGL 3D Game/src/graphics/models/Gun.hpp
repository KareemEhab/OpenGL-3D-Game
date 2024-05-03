#include "../Model.h"
#include "../../io/Camera.h"
#include "../../io/keyboard.h"

class Gun : public Model
{
public:
	Gun()
		: Model(BoundTypes::AABB, glm::vec3(0.0f), glm::vec3(1 / 300.0f), true) { }

	void render(Shader shader, float dt, Box *box, bool setModel = false)
	{
		// Set position
		glm::mat4 model = glm::mat4(1.0f);
		rb.pos = Camera::defaultCamera.getPos() 
			+ glm::vec3(Camera::defaultCamera.getFront() * 0.25f) 
			+ glm::vec3(Camera::defaultCamera.getUp() * -0.08f) 
			+ glm::vec3(Camera::defaultCamera.getRight() * 0.085f);
		model = glm::translate(model, rb.pos);

		float theta;

		// Rotate around cameraRight using dot product
		theta = acos(
			glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), Camera::defaultCamera.getFront())
			/ glm::length(Camera::defaultCamera.getUp())
			/ glm::length(Camera::defaultCamera.getFront())
		);
		model = glm::rotate(model, glm::radians(90.0f) - theta, Camera::defaultCamera.getRight()); // Offset by pi/2 radians bc angle between camFront and gunFront

		// Rotate around cameraUp using dot product
		glm::vec2 front2d = glm::vec2(Camera::defaultCamera.getFront().x, Camera::defaultCamera.getFront().z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), front2d) / glm::length(front2d));
		model = glm::rotate(model, Camera::defaultCamera.getFront().z < 0 ? theta : -theta, Camera::defaultCamera.getWorldUp());

		model = glm::scale(model, size);
		shader.setMat4("model", model);

		Model::render(shader, dt, box, false);
	}
};