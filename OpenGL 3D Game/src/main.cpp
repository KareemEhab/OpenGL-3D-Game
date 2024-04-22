#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "io/Joystick.h"
#include "io/Camera.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, double deltaTime);

float mixVal = 0.5f;

glm::mat4 transform = glm::mat4(1.0f);
Joystick mainJ(0);

unsigned int SCREEN_W = 800, SCREEN_H = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float x, y, z;
float theta = 45.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//
	GLFWwindow* window = glfwCreateWindow(SCREEN_W, SCREEN_H, "Window Title", NULL, NULL);
	if (window == NULL)
	{
		cout << "Could not create window" << endl;
		glfwTerminate();
		return -1;
	}
	// Set context for GLAD
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		glfwTerminate();
		return -1;
	}
	glViewport(0, 0, SCREEN_W, SCREEN_H);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	// Inputs
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	// Disable cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Shaders
	Shader shader("assets/vertex_core.glsl", "assets/fragment_core.glsl");
	Shader shader2("assets/vertex_core.glsl", "assets/fragment_core2.glsl");

	// Vertex array

	//float vertices[] = {
	//	// positions		colors				texture
	//	0.5f, 0.5f, 0.0f,	1.0f, 1.0f, 0.5f,	1.0f, 1.0f,	// top-right
	//	-0.5f, 0.5f, 0.0f,	0.5f, 1.0f, 0.75f,	0.0f, 1.0f,	// top-left		
	//	-0.5f, -0.5f, 0.0f, 0.6f, 1.0f, 0.2f,	0.0f, 0.0f,	// bottom-left	
	//	0.5f, -0.5f, 0.0f,	1.0f, 0.2f, 1.0f,	1.0f, 0.0f,	// bottom-right	
	//};

	//unsigned int indeces[] = {
	//	0, 1, 2,
	//	2, 3, 0
	//};

	float vertices[] = {
	// position				// tex
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// VAO, VBO, EBO
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO for use
	glBindVertexArray(VAO);

	// Bind VBO for use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set attribute pointer in shader
	// Position attribute (Location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Texture attribute (Location = 1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture
	unsigned int texture1, texture2, texture3;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	// Load Texture Image
	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("assets/image1.jpg", &width, &height, &nChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}

	stbi_image_free(data);

	shader.activate();
	shader.setInt("texture1", 0);

	// Texture 2
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	data = stbi_load("assets/image2.png", &width, &height, &nChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture2" << endl;
	}

	stbi_image_free(data);

	shader.activate();
	shader.setInt("texture2", 1);
	
	// Texture 3
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);

	data = stbi_load("assets/image3.png", &width, &height, &nChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture3" << endl;
	}

	stbi_image_free(data);

	shader.activate();
	shader.setInt("texture3", 2);

	// Transform
	//transform = glm::rotate(transform, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	shader.activate();
	shader.setMat4("transform", transform);
	shader2.activate();
	shader2.setMat4("transform", transform);

	float angle = 0.0f;
	float rotationSpeed = 0.005f;

	mainJ.update();
	if (mainJ.isPresent())
	{
		cout << mainJ.getName() << " is present." << endl;
	}
	else
	{
		cout << "No joystick present." << endl;
	}

	x = 0.0f;
	y = 0.0f;
	z = 3.0f;
	glm::mat4 model = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(window))
	{
		// Calculate Delta time
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		// Process Input
		processInput(window, deltaTime);

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture3);

		// Draw shapes
		glBindVertexArray(VAO);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-55.0f), glm::vec3(0.5f));
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.zoom), (float)SCREEN_W / (float)SCREEN_H, 0.1f, 100.0f);

		shader.activate();
		// Set uniform variables
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setFloat("mixVal", mixVal);

		//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		angle += rotationSpeed;
		angle = fmod(angle, 360.0f);

		glBindVertexArray(0);

		// Send new frame to window
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up the memory
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_W = width;
	SCREEN_H = height;
}

void processInput(GLFWwindow* window, double deltaTime)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE) || mainJ.buttonState(GLFW_JOYSTICK_BTN_RIGHT))
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (Keyboard::keyDown(GLFW_KEY_UP))
	{
		mixVal += .05f;
		if (mixVal > 1)
		{
			mixVal = 1.0f;
		}
	}
	if (Keyboard::key(GLFW_KEY_DOWN))
	{
		mixVal -= .05f;
		if (mixVal < 0)
		{
			mixVal = 0.0f;
		}
	}

	// Move camera
	if (Keyboard::key(GLFW_KEY_W))
	{
		camera.updateCameraPos(CameraDirection::FORWARD, deltaTime);
	}
	if (Keyboard::key(GLFW_KEY_S))
	{
		camera.updateCameraPos(CameraDirection::BACKWARD, deltaTime);
	}
	if (Keyboard::key(GLFW_KEY_A))
	{
		camera.updateCameraPos(CameraDirection::LEFT, deltaTime);
	}
	if (Keyboard::key(GLFW_KEY_D))
	{
		camera.updateCameraPos(CameraDirection::RIGHT, deltaTime);
	}
	if (Keyboard::key(GLFW_KEY_SPACE))
	{
		camera.updateCameraPos(CameraDirection::UP, deltaTime);
	}
	if (Keyboard::key(GLFW_KEY_LEFT_CONTROL))
	{
		camera.updateCameraPos(CameraDirection::DOWN, deltaTime);
	}

	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0)
	{
		cout << dy << endl;
		camera.updateCameraDirection(dx, dy);
	}

	double scrollDy = Mouse::getScrollDY();
	if(scrollDy != 0)
		camera.updateCameraZoom(scrollDy);

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