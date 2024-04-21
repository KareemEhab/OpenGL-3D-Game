#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Keyboard
{
public:
	// Key state callback
	static void keyCallback(GLFWwindow* winodw, int key, int scancode, int action, int mods);

	static bool key(int key);
	static bool keyChanged(int key);
	static bool keyUp(int key);
	static bool keyDown(int key);

private:
	static bool keys[];
	static bool keysChanged[];
};

