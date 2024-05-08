#pragma once
#include <glm/glm.hpp>
#include "../physics/RigidBody.h"

// Forward declaration
namespace Octree
{
	class node;
}

enum class BoundTypes : unsigned char
{
	AABB = 0x00, // Axis-aligned bounding box
	SPHERE = 0x01
};

class BoundingRegion
{
public:
	BoundTypes type;
	RigidBody* instance; // Pointer for quick access to instance
	Octree::node* cell; // Pointer for quick access to current octree node

	// Shpere values
	glm::vec3 center;
	float radius;

	glm::vec3 ogCenter;
	float ogRadius;

	// Bounding box values
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 ogMin;
	glm::vec3 ogMax;

	//	Constructors==========
	BoundingRegion(BoundTypes type = BoundTypes::AABB);

	// Sphere
	BoundingRegion(glm::vec3 center, float radius);

	// AABB
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	// Transform for instances
	void transform();

	// Calculate values for regions==========
	
	// Center
	glm::vec3 calculateCenter();
	
	// Dimensions
	glm::vec3 calculateDimensions();

	// Testing methods=========
	
	// Determine if point inside
	bool containsPoint(glm::vec3 pt);

	// Determine if region inside (Completely contains)
	bool containsRegion(BoundingRegion br);

	// Determine if region intersects (Partially contains)
	bool intersectsWith(BoundingRegion br);

	// Operator overload
	bool operator==(BoundingRegion br);
};

