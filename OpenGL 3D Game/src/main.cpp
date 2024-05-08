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

#include "algorithms/States.hpp"

#include "physics/Environment.h"

#include "Scene.h"

using namespace std;

Scene scene;

Camera cam(glm::vec3(0.0f));

void processInput(float deltaTime);
void launchItem(float deltaTime);

Joystick mainJ(0);

unsigned int SCREEN_W = 800, SCREEN_H = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Models Global======================
Sphere sphere(10);

int main()
{
	scene = Scene(3, 3, "OpenGL Engine", 800, 600);
	if (!scene.init())
	{
		cout << "Could not initialize window" << endl;
		glfwTerminate();
		return -1;
	}

	// Camera==========================
	scene.cameras.push_back(&cam);
	scene.activeCamera = 0;

	// Shaders==========================
	Shader lampShader("assets/instanced/instanced.vs", "assets/lamp.fs");
	Shader shader("assets/instanced/instanced.vs", "assets/object.fs");
	Shader boxShader("assets/instanced/box.vs", "assets/instanced/box.fs");

	// Models==========================
	Lamp lamp(4);
	scene.registerModel(&lamp);
	scene.registerModel(&sphere);

	// Load models
	scene.loadModels();

	// Lights=========================

	DirLight dirLight = { 
		glm::vec3(-0.2f, -1.0f, -0.3f), 
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
		glm::vec4(0.75f, 0.75f, 0.75f, 1.0f) 
	};
	scene.dirLight = &dirLight;

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

	PointLight pointLights[4];

	for (unsigned int i = 0; i < 4; i++)
	{
		pointLights[i] = {
			pointLightPositions[i],
			k0, k1, k2,
			ambient, diffuse, specular
		};
		scene.generateInstance(lamp.id, glm::vec3(0.25f), 1.0f, pointLightPositions[i]);
		scene.pointLights.push_back(&pointLights[i]);
		States::activate(&scene.activePointLights, i);
	}

	SpotLight spotLight = {
		cam.getPos(),
		cam.getFront(),
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
	}; // Using camera as spotlight

	scene.spotLights.push_back(&spotLight);
	scene.activeSpotLights = 1; // Just one so no need to do States::activate

	// Instantiate Instances
	scene.initInstances();

	// Joystick==================================
	mainJ.update();
	if (mainJ.isPresent())
		cout << mainJ.getName() << " is present." << endl;
	else
		cout << "No joystick present." << endl;

	// Main loop================================
	while (!scene.shouldClose())
	{
		// Calculate Delta time
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		// Process Input
		processInput(deltaTime);

		// Render
		scene.update();
		
		// Remove objects if they are too far from the camera
		stack<unsigned int> objectsToRemove;
		for (int i = 0; i < sphere.currentNoInstances; i++)
		{
			if (glm::length(cam.getPos() - sphere.instances[i].pos) > 250.0f)
				objectsToRemove.push(i);
		}

		while (objectsToRemove.size() > 0)
		{
			sphere.removeInstance(objectsToRemove.top());
			objectsToRemove.pop();
		}

		// Bullets
		if (sphere.currentNoInstances > 0)
		{
			scene.renderShader(shader);
			scene.renderInstances(sphere.id, shader, deltaTime);
		}

		// Lamps
		scene.renderShader(lampShader);
		scene.renderInstances(lamp.id, lampShader, deltaTime);

		// Send new frame to window
		scene.newFrame();
	}

	scene.cleanup();
	return 0;
}

void launchItem(float deltaTime)
{
	string id = scene.generateInstance(sphere.id, glm::vec3(0.1f), 1.0f, cam.getPos());
	if (id != "")
	{
		sphere.instances[scene.instances[id].second].transferEnergy(5000.0f, cam.getFront());
		sphere.instances[scene.instances[id].second].applyAcceleration(Environment::gravitationalAcceleration);
	}
}

// Inputs
void processInput(float deltaTime)
{
	scene.processInput(deltaTime);

	// Update flash light
	if (States::isIndexActive(&scene.activeSpotLights, 0))
	{
		scene.spotLights[0]->position = scene.getActiveCamera()->getPos();
		scene.spotLights[0]->direction = scene.getActiveCamera()->getFront();

	}

	if (Keyboard::key(GLFW_KEY_ESCAPE) || mainJ.buttonState(GLFW_JOYSTICK_BTN_RIGHT))
		scene.setShouldClose(true);

	if (Keyboard::keyDown(GLFW_KEY_L))
		States::toggleIndex(&scene.activeSpotLights, 0); // 0 for first spotlight

	if (Mouse::buttonDown(GLFW_MOUSE_BUTTON_1))
	{
		launchItem(deltaTime);
	}

	// Toggle point lights
	for (int i = 0; i < 4; i++) {
		if (Keyboard::keyDown(GLFW_KEY_1 + i)) {
			States::toggleIndex(&scene.activePointLights, i);
		}
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