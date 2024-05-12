#include "Scene.h"

unsigned int Scene::srcWidth = 0;
unsigned int Scene::srcHeight = 0;

string Scene::generateId()
{
	for (int i = currentId.length() - 1; i >= 0; i--)
	{
		if ((int)currentId[i] != (int)'z')
		{
			currentId[i] = (char)((int)currentId[i] + 1);
			break;
		}
		else
			currentId[i] = 'a';
	}

	return currentId;
}

void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Scene::srcWidth = width;
	Scene::srcHeight = height;
}

Scene::Scene()
	: currentId("aaaaaaaa") {}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight)
	: glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor), title(title), activeCamera(-1), activePointLights(0), activeSpotLights(0), currentId("aaaaaaaa")
{
	Scene::srcWidth = srcWidth;
	Scene::srcHeight = srcHeight;
	setWindowColor(0.1f, 0.15f, 0.15f, 1.0f);
}

bool Scene::init()
{
	glfwInit();
	
	// Set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Initialize window
	window = glfwCreateWindow(srcWidth, srcHeight, title, NULL, NULL);
	if (window == NULL)
		return false;
	glfwMakeContextCurrent(window);

	// Set GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return false;
	}

	// Setup screen
	glViewport(0, 0, srcWidth, srcHeight);

	// Callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	// Rendering params
	glEnable(GL_DEPTH_TEST); // Not render vertices not visible to the camera
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor

	//Init octree
	octree = new Octree::node(BoundingRegion(glm::vec3(-16.0f), glm::vec3(16.0f)));

	// Initialize freetype library
	if (FT_Init_FreeType(&ft))
	{
		cout << "Could not init FreeType library" << endl;
		return false;
	}

	// Insert font
	fonts.insert("comic", TextRenderer(32));
	if (!fonts["comic"].loadFont(ft, "assets/fonts/comic.ttf"))
	{
		cout << "Could not load SansSerifCollection font" << endl;
		return false;
	}

	// Setup lighting values
	variableLog["useBlinn"] = true;

	return true;
}


// Prepare for main loop after object generation, etc
void Scene::prepare(Box &box)
{
	octree->update(box);
}

void Scene::processInput(float dt)
{
	if (activeCamera != -1 && activeCamera < cameras.size())
	{
		// Set camera direction
		double dx = Mouse::getDX();
		double dy = Mouse::getDY();
		if (dx != 0 || dy != 0)
			cameras[activeCamera]->updateCameraDirection(dx, dy);
		
		// Set camera zoom
		double scrollDy = Mouse::getScrollDY();
		if (scrollDy != 0)
			cameras[activeCamera]->updateCameraZoom(Mouse::getScrollDY());
		
		// Move camera
		if (Keyboard::key(GLFW_KEY_W))
			cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);
		if (Keyboard::key(GLFW_KEY_S))
			cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);
		if (Keyboard::key(GLFW_KEY_A))
			cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);
		if (Keyboard::key(GLFW_KEY_D))
			cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);
		if (Keyboard::key(GLFW_KEY_SPACE))
			cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
		if (Keyboard::key(GLFW_KEY_LEFT_CONTROL))
			cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
		
		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(
			glm::radians(cameras[activeCamera]->getZoom()),	// FOV
			(float)srcWidth / (float)srcHeight,				// Aspect ratio
			0.1f, 100.0f									// Near and far bounds
		);
		textProjection = glm::ortho(0.0f, (float)srcWidth, 0.0f, (float)srcHeight);

		cameraPos = cameras[activeCamera]->getPos();

		// Update blinn param if necessary
		if(Keyboard::keyDown(GLFW_KEY_B))
			variableLog["useBlinn"] = !variableLog["useBlinn"].val<bool>();
	}
}

void Scene::update()
{
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::newFrame(Box &box)
{
	box.positions.clear();
	box.sizes.clear();

	// Process pending
	octree->processPending();
	octree->update(box);

	glfwSwapBuffers(window);	// Send new frame to window
	glfwPollEvents();			// For input
}

void Scene::renderShader(Shader shader, bool applyLighting)
{
	// Activate shader
	shader.activate();

	// Camera values
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);

	// Lighting
	if (applyLighting)
	{
		// Point lights
		int activeLights = 0;
		for (int i = 0; i < pointLights.size(); i++)
		{
			if (States::isIndexActive(&activePointLights, i))
				pointLights[i]->render(shader, activeLights++);
		}
		shader.setInt("noPointLights", activeLights);

		// Spot lights
		activeLights = 0;
		for (int i = 0; i < spotLights.size(); i++)
		{
			if (States::isIndexActive(&activeSpotLights, i))
				spotLights[i]->render(shader, activeLights++);
		}
		shader.setInt("noSpotLights", activeLights);

		// Directional light
		dirLight->render(shader);

		// Apply blinn if used
		shader.setBool("useBlinn", variableLog["useBlinn"].val<bool>());
	}
}

void Scene::renderInstances(string modelId, Shader shader, float dt)
{
	models[modelId]->render(shader, dt, this);
}

void Scene::renderText(string font, Shader shader, string text, float x, float y, glm::vec2 scale, glm::vec3 color)
{
	shader.activate();
	shader.setMat4("projection", textProjection);

	fonts[font].render(shader, text, x, y, scale, color);
}


void Scene::cleanup()
{
	models.traverse([](Model* model) -> void {
		model->cleanup();
	});

	// Clean up model and instances tries
	models.cleanup();
	instances.cleanup();

	fonts.traverse([](TextRenderer tr) -> void {
		tr.cleanup();
	});

	// Clean up fonts trie
	fonts.cleanup();

	octree->destroy();

	glfwTerminate();
}

bool Scene::shouldClose()
{
	return glfwWindowShouldClose(window);
}

Camera* Scene::getActiveCamera()
{
	return activeCamera >= 0 && activeCamera < cameras.size() ? cameras[activeCamera] : nullptr;
}

void Scene::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Scene::setWindowColor(float r, float g, float b, float a)
{
	bg[0] = r;
	bg[1] = g;
	bg[2] = b;
	bg[3] = a;
}

void Scene::registerModel(Model* model) 
{
	models.insert(model->id, model);
}

RigidBody* Scene::generateInstance(string modelId, glm::vec3 size, float mass, glm::vec3 pos) 
{
	RigidBody* rb = models[modelId]->generateInstance(size, mass, pos);
	if (rb) {
		// Successfully generated
		string id = generateId();
		rb->instanceId = id;
		instances.insert(id, rb);
		octree->addToPending(rb, models);
		return rb;
	}
	return nullptr;
}

void Scene::initInstances() 
{
	models.traverse([](Model* model) -> void {
		model->initInstances();
	});
}

void Scene::loadModels() 
{
	models.traverse([](Model* model) -> void {
		model->init();
	});
}

void Scene::removeInstance(string instanceId) 
{
	/*
		Remove all locations
		- Scene::instances
		- Model::instances
	*/
	string targetModel = instances[instanceId]->modelId;
	models[targetModel]->removeInstance(instanceId);
	instances[instanceId] = nullptr;
	instances.erase(instanceId);
}

void Scene::markForDeletion(string instanceId)
{
	States::activate(&instances[instanceId]->state, INSTANCE_DEAD);
	instancesToDelete.push_back(instances[instanceId]);
}

void Scene::clearDeadInstances()
{
	for (RigidBody* rb : instancesToDelete)
		removeInstance(rb->instanceId);

	instancesToDelete.clear();
}