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
#include <ft2build.h>

#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Light.h"
#include "graphics/Model.h"
#include "graphics/Cubemap.h"

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
	Shader textShader("assets/text.vs", "assets/text.fs");
	Shader skyboxShader("assets/skybox/skybox.vs", "assets/skybox/skybox.fs");
	/*Shader skyboxShader("assets/skybox/skybox.vs", "assets/skybox/sky.fs");
	skyboxShader.activate();
	skyboxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
	skyboxShader.set3Float("min", 0.945f, 1.000f, 0.682f);*/

	// Skybox===========================
	Cubemap skybox;
	skybox.init();
	skybox.loadTextures("assets/skybox");

	// Models==========================
	Gun gun(1);
	scene.registerModel(&gun);

	Lamp lamp(4);
	scene.registerModel(&lamp);
	scene.registerModel(&sphere);

	Box box;
	box.init();

	// Load models
	scene.loadModels();

	scene.generateInstance(gun.id, glm::vec3(0.05f), 1.0f, cam.getPos());

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

	scene.prepare(box);

	scene.variableLog["time"] = (double)0.0;

	// Main loop================================
	while (!scene.shouldClose())
	{
		// Calculate Delta time
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		scene.variableLog["time"] += deltaTime;
		scene.variableLog["fps"] = 1 / deltaTime;

		// Process Input
		processInput(deltaTime);

		// Render
		scene.update();

		// Render Skybox
		//skyboxShader.activate();
		//skyboxShader.setFloat("time", scene.variableLog["time"].val<float>());
		skybox.render(skyboxShader, &scene);
		
		// Remove objects if they are too far from the camera
		for (int i = 0; i < sphere.currentNoInstances; i++)
		{
			if (glm::length(cam.getPos() - sphere.instances[i]->pos) > 250.0f)
				scene.markForDeletion(sphere.instances[i]->instanceId);
		}

		// Bullets
		if (sphere.currentNoInstances > 0)
		{
			scene.renderShader(shader);
			scene.renderInstances(sphere.id, shader, deltaTime);
		}

		// Gun
		scene.renderShader(shader);
		scene.renderInstances(gun.id, shader, deltaTime);

		// Lamps
		scene.renderShader(lampShader);
		scene.renderInstances(lamp.id, lampShader, deltaTime);

		// Boxes
		scene.renderShader(boxShader, false);
		box.render(boxShader);

		// Render Text
		scene.renderText("comic", textShader, "Hello World", 50.0f, 50.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
		scene.renderText("comic", textShader, "Time: " + scene.variableLog["time"].dump(), 50.0f, 550.0f, glm::vec2(1.0f), glm::vec3(1.0f));
		scene.renderText("comic", textShader, "FPS: " + scene.variableLog["fps"].dump(), 50.0f, 500.0f, glm::vec2(1.0f), glm::vec3(1.0f));	

		// Send new frame to window
		scene.newFrame(box);
		scene.clearDeadInstances();
	}

	skybox.cleanup();
	scene.cleanup();
	return 0;
}

void launchItem(float deltaTime)
{
	RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.1f), 1.0f, cam.getPos());
	if (rb)
	{
		rb->transferEnergy(5000.0f, cam.getFront());
		rb->applyAcceleration(Environment::gravitationalAcceleration);
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