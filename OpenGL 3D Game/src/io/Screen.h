#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Screen
{
public:
	static unsigned int SCREEN_WIDTH;
	static unsigned int SCREEN_HEIGHT;

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	Screen();

	bool init();
	void setParameters();

	// Main loop
	void update();
	void newFrame();

	// Window closing accessor and modifier
	bool shouldClose();
	void setShouldClose(bool shouldClose);

private:
	GLFWwindow* window;
};

