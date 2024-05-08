#pragma once

#include <glm/glm.hpp>
#include <string>

#define INSTANCE_DEAD	(unsigned char) 0b00000001
#define INSTANCE_MOVED	(unsigned char) 0b00000010

using namespace std;

class RigidBody
{
public:
	unsigned char state;

	float mass;
	glm::vec3 pos;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 size;

	string modelId;
	string instanceId;

	bool operator==(RigidBody rb);
	bool operator==(string id);

	RigidBody();
	RigidBody(string modelId, glm::vec3 size = glm::vec3(1.0f), float mass = 1.0f, glm::vec3 pos = glm::vec3(0.0f));

	void update(float dt);

	void applyForce(glm::vec3 force);
	void applyForce(glm::vec3 direction, float magnitude);
	
	void applyAcceleration(glm::vec3 a);
	void applyAcceleration(glm::vec3 direction, float magnitude);

	void applyImpluse(glm::vec3 force, float dt);
	void applyImpluse(glm::vec3 direction, float magnitude, float dt);

	void transferEnergy(float joules, glm::vec3 direction);
};