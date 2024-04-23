#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "graphics/models/Cube.hpp"
#include "graphics/models/Lamp.hpp"

#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "io/Joystick.h"
#include "io/Camera.h"
#include "io/Screen.h"

using namespace std;

void processInput(double deltaTime);

Joystick mainJ(0);

unsigned int SCREEN_W = 800, SCREEN_H = 600;

Screen screen;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!screen.init())
	{
		cout << "Could not create window" << endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		glfwTerminate();
		return -1;
	}
	
	screen.setParameters();

	// Shaders
	Shader shader("assets/object.vs", "assets/object.fs");
	Shader lampShader("assets/object.vs", "assets/lamp.fs");

	// Models
	Cube cube(Material::gold, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f));
	cube.init();

	Lamp lamp(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(-1.0f, -0.5f, -0.5f), glm::vec3(0.25f));
	lamp.init();

	// Joystick
	mainJ.update();
	if (mainJ.isPresent())
		cout << mainJ.getName() << " is present." << endl;
	else
		cout << "No joystick present." << endl;

	glm::mat4 model = glm::mat4(1.0f);

	while (!screen.shouldClose())
	{
		// Calculate Delta time
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		// Process Input
		processInput(deltaTime);

		// Render
		screen.update();
		shader.activate();

		shader.set3Float("light.position", lamp.pos);
		shader.set3Float("viewPos", camera.getPos());
		shader.set3Float("light.ambient", lamp.ambient);
		shader.set3Float("light.diffuse", lamp.diffuse);
		shader.set3Float("light.specular", lamp.specular);

		// Draw
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCREEN_W / (float)SCREEN_H, 0.1f, 100.0f);

		// Set uniform variables
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		cube.render(shader);

		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		lamp.render(lampShader);

		// Send new frame to window
		screen.newFrame();
	}

	cube.cleanup();
	lamp.cleanup();

	glfwTerminate();
	return 0;
}

// Inputs
void processInput(double deltaTime)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE) || mainJ.buttonState(GLFW_JOYSTICK_BTN_RIGHT))
		screen.setShouldClose(true);

	// Move camera
	if (Keyboard::key(GLFW_KEY_W))
		camera.updateCameraPos(CameraDirection::FORWARD, deltaTime);
	if (Keyboard::key(GLFW_KEY_S))
		camera.updateCameraPos(CameraDirection::BACKWARD, deltaTime);
	if (Keyboard::key(GLFW_KEY_A))
		camera.updateCameraPos(CameraDirection::LEFT, deltaTime);
	if (Keyboard::key(GLFW_KEY_D))
		camera.updateCameraPos(CameraDirection::RIGHT, deltaTime);
	if (Keyboard::key(GLFW_KEY_SPACE))
		camera.updateCameraPos(CameraDirection::UP, deltaTime);
	if (Keyboard::key(GLFW_KEY_LEFT_CONTROL))
		camera.updateCameraPos(CameraDirection::DOWN, deltaTime);

	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0)
		camera.updateCameraDirection(dx, dy);

	double scrollDy = Mouse::getScrollDY();
	if(scrollDy != 0)
		camera.updateCameraZoom(scrollDy);

	// Joystick example
	/*mainJ.update();

	float lx = mainJ.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_X);
	float ly = -mainJ.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_Y);


	if (abs(lx) > 0.05f)
	{
		transform = glm::translate(transform, glm::vec3(lx / 10, 0.0f, 0.0f));
	}
	if (abs(ly) > 0.05f)
	{
		transform = glm::translate(transform, glm::vec3(0.0f, ly / 10.0f, 0.0f));
	}*/
}