#include "Bound.h"

//	Constructors
BoundingRegion::BoundingRegion(BoundTypes type)
	: type(type) {}

// Sphere
BoundingRegion::BoundingRegion(glm::vec3 center, float radius)
	: type(BoundTypes::SPHERE), center(center), radius(radius) {}

// AABB
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
	: type(BoundTypes::AABB), min(min), max(max) {}

// Calculate values for regions

// Center
glm::vec3 BoundingRegion::calculateCenter()
{
	return (type == BoundTypes::AABB ? (min + max) / 2.0f : center);
}

// Dimensions
glm::vec3 BoundingRegion::calculateDimensions()
{
	return (type == BoundTypes::AABB ? (max - min) : glm::vec3(2.0f * radius));
}

// Testing methods

// Determine if point inside
bool BoundingRegion::containsPoint(glm::vec3 pt)
{
	if (type == BoundTypes::AABB)
	{
		return (pt.x >= min.x) && (pt.x <= max.x) &&
			(pt.y >= min.y) && (pt.y <= max.y) &&
			(pt.z >= min.z) && (pt.z <= max.z);
	}
	else
	{
		// Sphere: distance must be less than radius
		// x^2 + y^2 + z ^2 <= r^2
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++)
			distSquared += (center[i] - pt[i]) * (center[i] - pt[i]);

		return distSquared <= (radius * radius);
	}
}

// Determine if region inside (Completely contains)
bool BoundingRegion::containsRegion(BoundingRegion br)
{
	// If br is a box, just has to contain min and max
	if (br.type == BoundTypes::AABB)
		return containsPoint(br.min) && containsPoint(br.max);
	// If both are spheres then combination of distance from centers and br.radius is less than radius
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE)
		return glm::length(center - br.center) + br.radius < radius;
	// If this is a box and br is a sphere
	else
	{
		// That means that the sphere is not completely inside of the box
		if (!containsPoint(br.center))
			return false;

		// Center is inside of the box
		for (int i = 0; i < 3; i++)
		{
			if (abs(max[i] - br.center[i]) < br.radius ||
				abs(br.center[i] - min[i]) < br.radius)
				return false;
		}
		return true;
	}
}

// Determine if region intersects (Partialy contains)
bool BoundingRegion::intersectsWith(BoundingRegion br)
{
	// Overlap on all axes
	if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) {
		// both boxes

		glm::vec3 rad = calculateDimensions() / 2.0f;				// "radius" of this box
		glm::vec3 radBr = br.calculateDimensions() / 2.0f;			// "radius" of br

		glm::vec3 center = calculateCenter();						// center of this box
		glm::vec3 centerBr = br.calculateCenter();					// center of br

		glm::vec3 dist = abs(center - centerBr);

		for (int i = 0; i < 3; i++) {
			if (dist[i] > rad[i] + radBr[i]) {
				// no overlap on this axis
				return false;
			}
		}

		// failed to prove wrong on each axis
		return true;
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
		// both spheres - distance between centers must be less than combined radius

		return glm::length(center - br.center) < (radius + br.radius);
	}
	else if (type == BoundTypes::SPHERE) {
		// this is a sphere, br is a box

		// determine if sphere is above top, below bottom, etc
		// find distance (squared) to the closest plane
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++) {
			if (center[i] < br.min[i]) {
				// beyond min
				distSquared += (br.min[i] - center[i]) * (br.min[i] * center[i]);
			}
			else if (center[i] > br.max[i]) {
				// beyond max
				distSquared += (center[i] - br.max[i]) * (center[i] - br.max[i]);
			}
			// else inside
		}

		return distSquared < (radius* radius);
	}
	else {
		// this is a box, br is a sphere
		// call algorithm for br (defined in preceding else if block)
		return br.intersectsWith(*this);
	}
}