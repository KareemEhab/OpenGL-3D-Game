#include "RigidBody.h"

bool RigidBody::operator==(RigidBody rb)
{
	return instanceId == rb.instanceId;
}

bool RigidBody::operator==(string id)
{
	return instanceId == id;
}

RigidBody::RigidBody() {}

RigidBody::RigidBody(string* modelId, glm::vec3 size, float mass, glm::vec3 pos)
	: modelId(modelId), size(size), mass(mass), pos(pos), velocity(0.0f), acceleration(0.0f){}

void RigidBody::update(float dt)
{
	pos += velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity += acceleration * dt;
}

void RigidBody::applyForce(glm::vec3 force)
{
	acceleration += force / mass;
}

void RigidBody::applyForce(glm::vec3 direction, float magnitude)
{
	applyForce(direction * magnitude);
}

void RigidBody::applyAcceleration(glm::vec3 a)
{
	acceleration += a;
}

void RigidBody::applyAcceleration(glm::vec3 direction, float magnitude)
{
	applyAcceleration(direction * magnitude);
}

void RigidBody::applyImpluse(glm::vec3 force, float dt)
{
	velocity += force / mass * dt;
}

void RigidBody::applyImpluse(glm::vec3 direction, float magnitude, float dt)
{
	applyImpluse(direction * magnitude, dt);
}

void RigidBody::transferEnergy(float joules, glm::vec3 direction)
{
	if (joules == 0)
		return;

	// Comes formula KE = 1/2 * m * v^2
	glm::vec3 deltaV = sqrt(2 * abs(joules) / mass) * direction;

	velocity += joules > 0 ? deltaV : -deltaV;
}