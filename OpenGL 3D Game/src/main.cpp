#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/anim.h>

#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Light.h"
#include "graphics/Model.h"

#include "graphics/models/Cube.hpp"
#include "graphics/models/Lamp.hpp"
#include "graphics/models/Gun.hpp"
#include "graphics/models/Sphere.hpp"
#include "graphics/models/Box.hpp"

#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "io/Joystick.h"
#include "io/Camera.h"
#include "io/Screen.h"

#include "physics/Environment.h"

using namespace std;

void processInput(float deltaTime);
void launchItem(float deltaTime);

Joystick mainJ(0);

unsigned int SCREEN_W = 800, SCREEN_H = 600;

Screen screen;
Camera Camera::defaultCamera(glm::vec3(0.0f, 0.0f, 0.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool flashLightIsOn = true;

SphereArray bullets;
Box box;

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

	// Shaders==========================
	Shader shader("assets/object.vs", "assets/object.fs");
	Shader lampShader("assets/object.vs", "assets/lamp.fs");
	Shader bulletShader("assets/instanced/instanced.vs", "assets/object.fs");
	Shader boxShader("assets/instanced/box.vs", "assets/instanced/box.fs");

	// Models==========================
	Gun gun;
	gun.loadModel("assets/models/m4a1/scene.gltf");

	bullets.init();
	box.init();

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	Cube cubes[10];
	for (unsigned int i = 0; i < 10; i++) {
		cubes[i] = Cube(cubePositions[i], glm::vec3(1.0f));
		cubes[i].init();
	}

	DirLight dirLight = { 
		glm::vec3(-0.2f, -1.0f, -0.3f), 
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
		glm::vec4(0.75f, 0.75f, 0.75f, 1.0f) 
	};

	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.09f;
	float k2 = 0.032f;

	/*Lamp lamps[4];
	for (unsigned int i = 0; i < 4; i++) {
		lamps[i] = Lamp(glm::vec3(1.0f),
			ambient, diffuse, specular,
			k0, k1, k2,
			pointLightPositions[i], glm::vec3(0.25f));
		lamps[i].init();
	}*/

	LampArray lamps;
	lamps.init();
	for (unsigned int i = 0; i < 4; i++)
	{
		lamps.lightInstances.push_back({
			pointLightPositions[i],
			k0, k1, k2,
			ambient, diffuse, specular
			});
	}

	//Cube cube(Material::gold, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f));
	//cube.init();

	//Lamp lamp(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, 0.07f, 0.032f, glm::vec3(-1.0f, -0.5f, -0.5f), glm::vec3(0.25f));
	//lamp.init();

	SpotLight spotLight = {
		Camera::defaultCamera.getPos(),
		Camera::defaultCamera.getFront(),
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
	}; // Using camera as spotlight

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
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		// Process Input
		processInput(deltaTime);

		// Render
		screen.update();
		shader.activate();
		shader.set3Float("viewPos", Camera::defaultCamera.getPos());

		bulletShader.activate();
		bulletShader.set3Float("viewPos", Camera::defaultCamera.getPos());

		shader.activate();
		dirLight.render(shader);
		bulletShader.activate();
		dirLight.render(bulletShader);
		
		for (int i = 0; i < 4; i++)
		{
			shader.activate();
			lamps.lightInstances[i].render(shader, i);
			bulletShader.activate();
			lamps.lightInstances[i].render(bulletShader, i);
		}
		shader.activate();
		shader.setInt("noPointLights", 4);
		bulletShader.activate();
		bulletShader.setInt("noPointLights", 4);

		// Spotlight
		if (flashLightIsOn)
		{
			spotLight.position = Camera::defaultCamera.getPos();
			spotLight.direction = Camera::defaultCamera.getFront();
			shader.activate();
			spotLight.render(shader, 0);
			shader.setInt("noSpotLights", 1);
			bulletShader.activate();
			spotLight.render(bulletShader, 0);
			bulletShader.setInt("noSpotLights", 1);
		}
		else
		{
			shader.activate();
			shader.setInt("noSpotLights", 0);
			bulletShader.activate();
			bulletShader.setInt("noSpotLights", 0);
		}

		// Draw
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = Camera::defaultCamera.getViewMatrix();
		projection = glm::perspective(glm::radians(Camera::defaultCamera.getZoom()), (float)SCREEN_W / (float)SCREEN_H, 0.1f, 100.0f);

		// Set uniform variables
		shader.activate();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		// Memory optimization
		stack<int> removeObjects;
		for (int i = 0; i < bullets.instances.size(); i++)
		{
			if (glm::length(Camera::defaultCamera.getPos() - bullets.instances[i].pos) > 300.0f)
				removeObjects.push(i);
		}
		for (int i = 0; i < removeObjects.size(); i++)
		{
			bullets.instances.erase(bullets.instances.begin() + removeObjects.top());
			removeObjects.pop();
		}

		// Draw cubes
		for (int i = 0; i < 10; i++)
			cubes[i].render(shader, deltaTime);
		
		// Draw Gun
		shader.activate();
		gun.render(shader, deltaTime);

		// Draw Sphere
		bulletShader.activate();
		if (bullets.instances.size() > 0)
		{
			bulletShader.setMat4("view", view);
			bulletShader.setMat4("projection", projection);
			bullets.render(bulletShader, deltaTime);
		}

		// Lamps
		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		// Draw Lamps
		lamps.render(lampShader, deltaTime);

		// Draw boxes
		if (box.offsets.size() > 0)
		{
			// Instances exist
			boxShader.activate();
			boxShader.setMat4("view", view);
			boxShader.setMat4("projection", projection);
			box.render(boxShader);
		}

		// Send new frame to window
		screen.newFrame();
	}

	for (int i = 0; i < 10; i++)
		cubes[i].cleanup();
	gun.cleanup();
	bullets.cleanup();
	lamps.cleanup();
	box.cleanup();

	glfwTerminate();
	return 0;
}

void launchItem(float deltaTime)
{
	RigidBody rb(1.0f, Camera::defaultCamera.getPos());
	rb.applyAcceleration(Environment::gravitationalAcceleration);
	rb.transferEnergy(5000.f, Camera::defaultCamera.getFront());
	//rb.applyImpluse(Camera::defaultCamera.getFront(), 5000.0f, deltaTime);
	bullets.instances.push_back(rb);
}

// Inputs
void processInput(float deltaTime)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE) || mainJ.buttonState(GLFW_JOYSTICK_BTN_RIGHT))
		screen.setShouldClose(true);

	// Move camera
	if (Keyboard::key(GLFW_KEY_W))
		Camera::defaultCamera.updateCameraPos(CameraDirection::FORWARD, deltaTime);
	if (Keyboard::key(GLFW_KEY_S))
		Camera::defaultCamera.updateCameraPos(CameraDirection::BACKWARD, deltaTime);
	if (Keyboard::key(GLFW_KEY_A))
		Camera::defaultCamera.updateCameraPos(CameraDirection::LEFT, deltaTime);
	if (Keyboard::key(GLFW_KEY_D))
		Camera::defaultCamera.updateCameraPos(CameraDirection::RIGHT, deltaTime);
	if (Keyboard::key(GLFW_KEY_SPACE))
		Camera::defaultCamera.updateCameraPos(CameraDirection::UP, deltaTime);
	if (Keyboard::key(GLFW_KEY_LEFT_CONTROL))
		Camera::defaultCamera.updateCameraPos(CameraDirection::DOWN, deltaTime);

	if (Keyboard::keyDown(GLFW_KEY_L))
		flashLightIsOn = !flashLightIsOn;

	if (Keyboard::keyDown(GLFW_KEY_I))
	{
		box.offsets.push_back(glm::vec3(box.offsets.size() * 1.0f));
		box.sizes.push_back(glm::vec3(box.sizes.size() * 0.5f));
	}

	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0)
		Camera::defaultCamera.updateCameraDirection(dx, dy);

	double scrollDy = Mouse::getScrollDY();
	if(scrollDy != 0)
		Camera::defaultCamera.updateCameraZoom(scrollDy);

	if (Mouse::buttonDown(GLFW_MOUSE_BUTTON_1))
	{
		launchItem(deltaTime);
	}

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