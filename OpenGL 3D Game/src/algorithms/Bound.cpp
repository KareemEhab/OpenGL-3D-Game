#include "Bound.h"
#include "Octree.h"

//	Constructors
BoundingRegion::BoundingRegion(BoundTypes type)
	: type(type) {}

// Sphere
BoundingRegion::BoundingRegion(glm::vec3 center, float radius)
	: type(BoundTypes::SPHERE), center(center), ogCenter(center), radius(radius), ogRadius(radius) {}

// AABB
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
	: type(BoundTypes::AABB), min(min), max(max), ogMin(min), ogMax(max) {}

// Calculate values for regions

// Transform for instance
void BoundingRegion::transform()
{
	if (instance)
	{
		if(type == BoundTypes::AABB)
		{
			min = ogMin * instance->size + instance->pos;
			max = ogMax * instance->size + instance->pos;
		}
		else
		{
			center = ogCenter * instance->size + instance->pos;
			radius = ogRadius * instance->size.x;
		}
	}
}

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
bool BoundingRegion::intersectsWith(BoundingRegion br) {
	// overlap on all axes

	if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) 
	{
		// both boxes

		glm::vec3 rad = calculateDimensions() / 2.0f;				// "radius" of this box
		glm::vec3 radBr = br.calculateDimensions() / 2.0f;			// "radius" of br

		glm::vec3 center = calculateCenter();						// center of this box
		glm::vec3 centerBr = br.calculateCenter();					// center of br

		glm::vec3 dist = abs(center - centerBr);

		for (int i = 0; i < 3; i++) 
		{
			if (dist[i] > rad[i] + radBr[i]) 
			{
				// no overlap on this axis
				return false;
			}
		}

		// failed to prove wrong on each axis
		return true;
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) 
	{
		// both spheres - distance between centers must be less than combined radius
		return glm::length(center - br.center) < (radius + br.radius);
	}
	else if (type == BoundTypes::SPHERE) 
	{
		// this is a sphere, br is a box
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++) 
		{
			// determine closest side
			float closestPt = std::max(br.min[i], std::min(center[i], br.max[i]));
			// add distance
			distSquared += (closestPt - center[i]) * (closestPt - center[i]);
		}

		return distSquared < (radius* radius);
	}
	else 
	{
		// this is a box, br is a sphere
		// call algorithm for br (defined in preceding else if block)
		return br.intersectsWith(*this);
	}
}

bool BoundingRegion::operator==(BoundingRegion br)
{
	if (type != br.type)
		return false;

	if (type == BoundTypes::AABB)
		return min == br.min && max == br.max;
	else
		return center == br.center && radius == br.radius;
}