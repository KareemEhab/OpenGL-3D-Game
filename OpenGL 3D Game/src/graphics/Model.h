#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "mesh.h"

#include "models/Box.hpp"

#include "../physics/RigidBody.h"
#include "../algorithms/Bound.h"
#include "../Scene.h"

#define DYNAMIC				(int)1
#define CONST_INSTANCES		(int)2
#define NO_TEX				(int)4

using namespace std;

class Scene; // Forward decleration (As scene has model class in it so to avoid circular definition)

class Model 
{
public:
	string id;
	RigidBody rb;
	glm::vec3 size;

	BoundTypes boundType;

	vector<Mesh> meshes;

	vector<RigidBody> instances;
	int maxNoInstances;
	int currentNoInstances;

	int switches;

	Model(string id, BoundTypes boundType, unsigned int maxNoInstances, unsigned int flags = 0);
	void loadModel(std::string path);

	virtual void init();

	unsigned int generateInstance(glm::vec3 size, float mass, glm::vec3 pos);

	void initInstances();

	void removeInstance(unsigned int idx);

	unsigned int getIdx(std::string id);

	virtual void render(Shader shader, float dt, Scene* scene, bool setModel = true);

	void cleanup();

	void removeInstances(int idx);

protected:
	bool noTex;
	
	string directory;

	vector<Texture> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

	// VBOs for positions and sizes
	BufferObject posVBO;
	BufferObject sizeVBO;
};

#endif