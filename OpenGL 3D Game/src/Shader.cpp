#include "Shader.h"


Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	int success;
	char infolog[512];

	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infolog);
		cout << "Linking error:" << endl << infolog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate()
{
	glUseProgram(id);
}

string Shader::loadShaderSrc(const char* filename)
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

GLuint Shader::compileShader(const char* filepath, GLenum type)
{
	int success;
	char infolog[512];

	GLuint currentShader = glCreateShader(type);
	string shaderSrc = loadShaderSrc(filepath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(currentShader, 1, &shader, NULL);
	glCompileShader(currentShader);


	// Catch error
	glGetShaderiv(currentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(currentShader, 512, NULL, infolog);
		cout << "Error compiling shader: " << filepath << endl << infolog << endl;
	}

	return currentShader;
}

void Shader::setMat4(const string& name, glm::mat4 val)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}