#include "../Model.h"
#include "../../io/Camera.h"

class Gun : public Model
{
public:
	Gun()
		: Model(glm::vec3(0.0f), glm::vec3(0.05f), true) { }

	void render(Shader shader, bool setModel = false)
	{
		glm::mat4 model = glm::mat4(1.0f);
		pos = Camera::defaultCamera.getPos();
		model = glm::scale(model, size);
		shader.setMat4("model", model);

		Model::render(shader, false);
	}
};