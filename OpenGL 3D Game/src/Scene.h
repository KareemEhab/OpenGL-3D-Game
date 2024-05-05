#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>

#include "graphics/Light.h"
#include "graphics/Shader.h"

#include "io/Camera.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"

#include "algorithms/States.hpp"

using namespace std;

#pragma once
class Scene
{
public:
	// Callbacks
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	Scene() {}
	Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight);

	bool init();

	// Main loop methods
	void processInput(float dt);
	void update();		// Update screen before each frame
	void newFrame();	// Update screen after each frame
	void render(Shader shader, bool applyLighting = true); // Set uniform shader variables (eg: lighting)

	void cleanup();

	bool shouldClose();
	Camera* getActiveCamera();
	void setShouldClose(bool shouldClose);
	void setWindowColor(float r, float g, float b, float a);

	// Lights
	vector<PointLight*> pointLights;
	unsigned int activePointLights;
	
	vector<SpotLight*> spotLights;
	unsigned int activeSpotLights;

	DirLight* dirLight;
	bool dirLightActive;

	// Camera
	vector<Camera*> cameras;
	unsigned int activeCamera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPos;

protected:
	GLFWwindow* window;

	// Window values
	const char* title;
	static unsigned int srcWidth;
	static unsigned int srcHeight;
	float bg[4]; // Background color

	int glfwVersionMajor;
	int glfwVersionMinor;
};

