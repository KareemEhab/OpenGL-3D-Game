#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraDirection {
	NONE = 0,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	Camera(glm::vec3 position);

	void updateCameraDirection(double dx, double dy);
	void updateCameraPos(CameraDirection dir, double dt);
	void updateCameraZoom(double dy);

	float getZoom() { return zoom; }
	glm::vec3 getPos() { return cameraPos; }
	glm::vec3 getFront() { return cameraFront; }
	glm::vec3 getUp() { return cameraUp; }
	glm::vec3 getRight() { return cameraRight; }
	glm::vec3 getWorldUp() { return worldUp; }

	glm::mat4 getViewMatrix();


	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::vec3 worldUp;

	float yaw;
	float pitch;
	float speed;
	float zoom;

	void updateCameraVectors();
};

