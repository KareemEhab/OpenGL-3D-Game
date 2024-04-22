#include "Screen.h"


#include "Keyboard.h"
#include "Mouse.h"

unsigned int Screen::SCREEN_WIDTH = 800;
unsigned int Screen::SCREEN_HEIGHT = 600;

void Screen::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

Screen::Screen()
	: window(nullptr) {}

bool Screen::init()
{
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window Title", NULL, NULL);
	if (!window)
		return false;

	glfwMakeContextCurrent(window);
	return true;
}

void Screen::setParameters()
{
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, Screen::framebufferSizeCallback);

	// Enable Z depth test
	glEnable(GL_DEPTH_TEST);

	// Inputs
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	// Disable cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Screen::update()
{
	// Render
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::newFrame()
{
	glfwSwapBuffers(window);
	glfwPollEvents(); // To process inputs
}

bool Screen::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Screen::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}