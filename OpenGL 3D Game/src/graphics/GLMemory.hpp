#ifndef GLMEMORY_HPP
#define GLMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

using namespace std;

// Class for buffer objects VBOs, EBOs, etc...
class BufferObject
{
public:
	// Value/Location
	GLuint val;
	GLenum type; // GL_ARRAY BUFFER || GL_ELEMENT_ARRAY_BUFFER, etc
	
	BufferObject(){}
	BufferObject(GLenum type)
		: type(type){}

	void generate()
	{
		glGenBuffers(1, &val);
	}

	// Bind object
	void bind()
	{
		glBindBuffer(type, val);
	}

	// Set data (glBufferData)
	template<typename T>
	void setData(GLuint noElements, T* data, GLenum usage)
	{
		glBufferData(type, noElements * sizeof(T), data, usage);
	}

	// Update data (glBufferSubData)
	template <typename T>
	void updateData(GLintptr offset, GLuint noElements, T* data)
	{
		glBufferSubData(type, offset, noElements * sizeof(T), data);
	}

	// Set attribute pointers
	template <typename T>
	void setAttribPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor = 0)
	{
		glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(offset * sizeof(T)));
		glEnableVertexAttribArray(idx);
		if (divisor > 0)
			glVertexAttribDivisor(idx, divisor);
	}

	// Clear buffer objects
	void clear()
	{
		glBindBuffer(type, 0);
	}

	// Cleanup
	void cleanup()
	{
		glDeleteBuffers(1, &val);
	}
};

// Class for array objects (VAO)
class ArrayObject
{
public:
	// Value/Location
	GLuint val;

	map<const char*, BufferObject> buffers;

	// Get buffer
	BufferObject& operator[](const char* key)
	{
		return buffers[key];
	}

	void generate()
	{
		glGenVertexArrays(1, &val);
	}

	void bind()
	{
		glBindVertexArray(val);
	}

	void draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instancecount = 1)
	{
		glDrawElementsInstanced(mode, count, type, (void*)indices, instancecount);
	}

	void cleanup()
	{
		glDeleteVertexArrays(1, &val);
		for (auto& pair : buffers)
		{
			pair.second.cleanup();
		}
	}

	// Clear array object (bind 0)
	static void clear()
	{
		glBindVertexArray(0);
	}
};

#endif