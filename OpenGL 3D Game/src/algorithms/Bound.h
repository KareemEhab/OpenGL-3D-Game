#include <glm/glm.hpp>

#pragma once

enum class BoundTypes
{
	AABB = 0x00, // Axis-aligned bounding box
	SPHERE = 0x01
};

class BoundingRegion
{
public:
	BoundTypes type;

	// Shpere values
	glm::vec3 center;
	float radius;

	// Bounding box values
	glm::vec3 min;
	glm::vec3 max;

	//	Constructors
	BoundingRegion(BoundTypes type);

	// Sphere
	BoundingRegion(glm::vec3 center, float radius);

	// AABB
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	// Calculate values for regions
	
	// Center
	glm::vec3 calculateCenter();
	
	// Dimensions
	glm::vec3 calculateDimensions();

	// Testing methods
	
	// Determine if point inside
	bool containsPoint(glm::vec3 pt);

	// Determine if region inside (Completely contains)
	bool containsRegion(BoundingRegion br);

	// Determine if region intersects (Partially contains)
	bool intersectsWith(BoundingRegion br);
};

