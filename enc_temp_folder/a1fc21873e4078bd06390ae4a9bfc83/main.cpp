#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
string loadShaderSrc(const char* filename);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//
	GLFWwindow* window = glfwCreateWindow(800, 600, "Window Title", NULL, NULL);
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
	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Shaders
	int success;
	char infolog[512];

	// Compile vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	string vertShaderSrc = loadShaderSrc("assets/vertex_core.glsl");
	const GLchar* vertShader = vertShaderSrc.c_str();
	glShaderSource(vertexShader, 1, &vertShader, NULL);
	glCompileShader(vertexShader);

	// Catch error
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		cout << "Error compiling vertex shader:" << endl << infolog << endl;
	}

	// Compile fragment shader
	unsigned int fragmentShader[2];

	// Fragment Shader 1
	fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);
	string fragShaderSrc = loadShaderSrc("assets/fragment_core.glsl");
	const GLchar* fragShader = fragShaderSrc.c_str();
	glShaderSource(fragmentShader[0], 1, &fragShader, NULL);
	glCompileShader(fragmentShader[0]);

	// Catch error
	glGetShaderiv(fragmentShader[0], GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader[0], 512, NULL, infolog);
		cout << "Error compiling fragment shader:" << endl << infolog << endl;
	}

	// Fragment Shader 2
	fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER);
	fragShaderSrc = loadShaderSrc("assets/fragment_core2.glsl");
	fragShader = fragShaderSrc.c_str();
	glShaderSource(fragmentShader[1], 1, &fragShader, NULL);
	glCompileShader(fragmentShader[1]);

	// Catch error
	glGetShaderiv(fragmentShader[0], GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader[0], 512, NULL, infolog);
		cout << "Error compiling fragment shader:" << endl << infolog << endl;
	}

	// Shader Programs
	unsigned int shaderProgram[2];

	// Shader Program 1
	shaderProgram[0] = glCreateProgram();
	glAttachShader(shaderProgram[0], vertexShader);
	glAttachShader(shaderProgram[0], fragmentShader[0]);
	glLinkProgram(shaderProgram[0]);

	// Catch error
	glGetProgramiv(shaderProgram[0], GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram[0], 512, NULL, infolog);
		cout << "Linking error:" << endl << infolog << endl;
	}

	//Shader Program 2
	shaderProgram[1] = glCreateProgram();
	glAttachShader(shaderProgram[1], vertexShader);
	glAttachShader(shaderProgram[1], fragmentShader[1]);
	glLinkProgram(shaderProgram[1]);

	// Catch error
	glGetProgramiv(shaderProgram[1], GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram[1], 512, NULL, infolog);
		cout << "Linking error:" << endl << infolog << endl;
	}

	// Delete shaders after binding to programs
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader[0]);
	glDeleteShader(fragmentShader[1]);

	// Vertex array
	float vertices[] = {
		0.5f, 0.5f, 0.0f, // top-right
		-0.5f, 0.5f, 0.0f, // top-left
		-0.5f, -0.5f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.0f, // bottom-right
	};

	unsigned int indeces[] = {
		0, 1, 2,
		2, 3, 0
	};

	// VAO, VBO, EBO
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO for use
	glBindVertexArray(VAO);

	// Bind VBO for use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set up EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW);

	while (!glfwWindowShouldClose(window))
	{
		// Process Input
		processInput(window);

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw shapes
		glBindVertexArray(VAO);

		// First triangle
		glUseProgram(shaderProgram[0]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		
		// Second triangle
		glUseProgram(shaderProgram[1]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(unsigned int)));

		// Send new frame to window
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

string loadShaderSrc(const char* filename)
{
	ifstream file;
	stringstream buf;

	string ret = "";

	file.open(filename);

	if (file.is_open())
	{
		buf << file.rdbuf();
		ret = buf.str();
	}
	else
	{
		cout << "Could not open " << filename << endl;
	}
	file.close();

	return ret;
}