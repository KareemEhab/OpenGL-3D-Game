#include "Scene.h"

unsigned int Scene::srcWidth = 0;
unsigned int Scene::srcHeight = 0;

void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Scene::srcWidth = width;
	Scene::srcHeight = height;
}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight)
	: glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor), title(title), activeCamera(-1), activePointLights(0), activeSpotLights(0)
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor
}

void Scene::processInput(float dt)
{
	if (activeCamera != -1 && activeCamera < cameras.size())
	{
		// Set camera direction
		//if (Mouse::getDX() != 0 || Mouse::getDY() != 0)
			cameras[activeCamera]->updateCameraDirection(Mouse::getDX(), Mouse::getDY());
		// Set camera zoom
		//if (Mouse::getScrollDY() != 0)
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

		cameraPos = cameras[activeCamera]->getPos();
	}
}

void Scene::update()
{
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::newFrame()
{
	glfwSwapBuffers(window);	// Send new frame to window
	glfwPollEvents();			// For input
}

void Scene::render(Shader shader, bool applyLighting)
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
			if (States::isActive(&activePointLights, i))
				pointLights[i]->render(shader, activeLights++);
		}
		shader.setInt("noPointLights", activeLights);

		// Spot lights
		activeLights = 0;
		for (int i = 0; i < spotLights.size(); i++)
		{
			if (States::isActive(&activeSpotLights, i))
				spotLights[i]->render(shader, activeLights++);
		}
		shader.setInt("noSpotLights", activeLights);

		// Directional light
		dirLight->render(shader);
	}
}

void Scene::cleanup()
{
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