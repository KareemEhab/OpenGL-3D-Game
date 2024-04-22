#include "Mesh.h"

vector<Vertex> Vertex::genList(float* vertices, int noVertices)
{
	vector<Vertex> ret(noVertices);

	int stride = sizeof(Vertex) / sizeof(float);

	for (int i = 0; i < noVertices; i++)
	{
		ret[i].pos = glm::vec3{
			vertices[i * stride + 0],
			vertices[i * stride + 1],
			vertices[i * stride + 2],
		};

		ret[i].texCoord = glm::vec2{
			vertices[i * stride + 3],
			vertices[i * stride + 4],
		};
	}

	return ret;
}

Mesh::Mesh()
{

}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indeces, vector<Texture> textures)
	: vertices(vertices), indeces(indeces), textures(textures)
{
	setup();
}

void Mesh::render(Shader shader)
{
	// Textures
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		shader.setInt(textures[i].name, textures[i].id);
		glActiveTexture(GL_TEXTURE0 + i);
		textures[i].bind();
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indeces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::cleanup()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::setup()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeces.size() * sizeof(unsigned int), &indeces[0], GL_STATIC_DRAW);

	// Set vertex attribute pointers
	// vertex.position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex.texCoord
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glBindVertexArray(0);
}