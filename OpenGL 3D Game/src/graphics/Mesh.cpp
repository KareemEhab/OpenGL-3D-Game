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

		ret[i].normal = glm::vec3{
			vertices[i * stride + 3],
			vertices[i * stride + 4],
			vertices[i * stride + 5],
		};

		ret[i].texCoord = glm::vec2{
			vertices[i * stride + 6],
			vertices[i * stride + 7],
		};
	}

	return ret;
}

// Default constructor
Mesh::Mesh() {}

// Textured object
Mesh::Mesh(BoundingRegion br, vector<Texture> textures)
	: br(br), textures(textures), noTex(false) {}

// Material object
Mesh::Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec)
	: br(br), diffuse(diff), specular(spec), noTex(true) {}

// Load vertex and index data
void Mesh::loadData(vector<Vertex> _vertices, vector<unsigned int> _indices) 
{
	this->vertices = _vertices;
	this->indeces = _indices;

	// bind VAO
	VAO.generate();
	VAO.bind();

	// generate/set EBO
	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].generate();
	VAO["EBO"].bind();
	VAO["EBO"].setData<GLuint>(this->indeces.size(), &this->indeces[0], GL_STATIC_DRAW);

	// load data into vertex buffers
	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["VBO"].generate();
	VAO["VBO"].bind();
	VAO["VBO"].setData<Vertex>(this->vertices.size(), &this->vertices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	VAO["VBO"].bind();
	// vertex Positions
	VAO["VBO"].setAttribPointer<GLfloat>(0, 3, GL_FLOAT, 8, 0);
	// normal ray
	VAO["VBO"].setAttribPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
	// vertex texture coords
	VAO["VBO"].setAttribPointer<GLfloat>(2, 3, GL_FLOAT, 8, 6);

	VAO["VBO"].clear();

	ArrayObject::clear();
}

void Mesh::render(Shader shader, unsigned int noOfInstances)
{
	if (noTex)
	{
		// materials
		shader.set4Float("material.diffuse", diffuse);
		shader.set4Float("material.specular", specular);
		shader.setInt("noTex", 1);
	}
	else {
		shader.setInt("noTex", 0);
		// textures
		unsigned int diffuseIdx = 0;
		unsigned int specularIdx = 0;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			// Activate texture
			glActiveTexture(GL_TEXTURE0 + i);

			// Retrieve texture info
			string name;
			switch (textures[i].type) {
			case aiTextureType_DIFFUSE:
				name = "diffuse" + to_string(diffuseIdx++);
				break;
			case aiTextureType_SPECULAR:
				name = "specular" + to_string(specularIdx++);
				break;
			}

			// Set the shader value
			shader.setInt(name, i);
			// Bind texture
			textures[i].bind();
		}
	}
	VAO.bind();
	VAO.draw(GL_TRIANGLES, indeces.size(), GL_UNSIGNED_INT, 0, noOfInstances);
	ArrayObject::clear();

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setup()
{
	// Bind VAO
	VAO.generate();
	VAO.bind();

	// EBO
	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].generate();
	VAO["EBO"].bind();
	VAO["EBO"].setData<GLuint>(indeces.size(), &indeces[0], GL_STATIC_DRAW);

	// VBO
	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["VBO"].generate();
	VAO["VBO"].bind();
	VAO["VBO"].setData<Vertex>(vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Set vertex attrib pointers
	// positions
	VAO["VBO"].setAttribPointer<GLfloat>(0, 3, GL_FLOAT, 8, 0);
	// normal
	VAO["VBO"].setAttribPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
	// texture
	VAO["VBO"].setAttribPointer<GLfloat>(2, 2, GL_FLOAT, 8, 6);

	VAO["VBO"].clear();
	ArrayObject::clear();
}

void Mesh::cleanup()
{
	VAO.cleanup();
}