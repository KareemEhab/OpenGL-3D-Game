#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <jsoncpp/json.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphics/Light.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "graphics/Text.h"
#include "graphics/models/Box.hpp"

#include "io/Camera.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"

#include "algorithms/States.hpp"
#include "algorithms/Trie.hpp"
#include "algorithms/Octree.h"

using namespace std;

// Forward decleration
namespace Octree {
	class node;
}
class Model;

class Scene
{
public:
	trie::Trie<Model*> models;
	trie::Trie<RigidBody*> instances;

	vector<RigidBody*> instancesToDelete;

	Octree::node* octree;

	// Map for logged variables
	jsoncpp::json variableLog;

	// Freetype library
	FT_Library ft;
	trie::Trie<TextRenderer> fonts;

	// Callbacks
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	Scene();
	Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight);

	bool init();

	void prepare(Box &box);

	// Main loop methods
	void processInput(float dt);
	void update();		// Update screen before each frame
	void newFrame(Box &box);	// Update screen after each frame
	void renderShader(Shader shader, bool applyLighting = true); // Set uniform shader variables (eg: lighting)
	void renderInstances(string modelId, Shader shader, float dt); \
	void renderText(string font, Shader shader, string text, float x, float y, glm::vec2 scale, glm::vec3 color);

	void cleanup();

	bool shouldClose();
	Camera* getActiveCamera();
	void setShouldClose(bool shouldClose);
	void setWindowColor(float r, float g, float b, float a);

	// Model/instance methods
	void registerModel(Model* model);
	RigidBody* generateInstance(string modelId, glm::vec3 size, float mass, glm::vec3 pos);
	void initInstances();
	void loadModels();
	void removeInstance(string instanceId);
	void markForDeletion(string instanceId);
	void clearDeadInstances();

	string currentId;
	string generateId();

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
	glm::mat4 textProjection;
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

