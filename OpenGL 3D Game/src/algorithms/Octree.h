#pragma once
#define NO_CHILDREN 8
#define MIN_BOUNDS 0.5

#include <vector>
#include <queue>
#include <stack>

#include "List.hpp"
#include "States.hpp"
#include "Bound.h"
#include "Trie.hpp"

#include "../graphics/Model.h"

using namespace std;

// Forward decleration
class Model;
class BoundingRegion;
class Box;

namespace Octree 
{
	//enum to represent octants
	enum class Octant : unsigned char 
	{
		O1 = 0x01,	// = 0b00000001
		O2 = 0x02,	// = 0b00000010
		O3 = 0x04,	// = 0b00000100
		O4 = 0x08,	// = 0b00001000
		O5 = 0x10,	// = 0b00010000
		O6 = 0x20,	// = 0b00100000
		O7 = 0x40,	// = 0b01000000
		O8 = 0x80	// = 0b10000000
	};

	// calculate bounds of specified quadrant in bounding region
	void calculateBounds(BoundingRegion &out, Octant octant, BoundingRegion parentRegion);

	/*
		class to represent each node in the octree
	*/
	class node 
	{
	public:
		node* parent; // parent pointer
		node* children[NO_CHILDREN]; // array of children (8)

		unsigned char activeOctants; // switch for active octants

		bool hasChildren = false; // if node has children

		bool treeReady = false; // if tree is ready
		bool treeBuilt = false; // if tree is built
		short maxLifespan = 8;
		short currentLifespan = -1;

		vector<BoundingRegion> objects; // list of objects in node
		queue<BoundingRegion> queue; // queue of objects to be dynamically inserted

		BoundingRegion region; // region of bounds of cell (AABB)

		// default constructor
		node();

		// initialize with bounds (no objects yet)
		node(BoundingRegion bounds);

		// initialize with bounds and list of objects
		node(BoundingRegion bounds, std::vector<BoundingRegion> objectList);

		void addToPending(RigidBody* instance, trie::Trie<Model*> models);

		// build tree (called during initialization)
		void build();

		// update objects in tree (called during each iteration of main loop)
		void update(Box &box);

		// process pending queue
		void processPending();

		// dynamically insert object into node
		bool insert(BoundingRegion obj);

		// check collisions with all objects in node
		void checkCollisionsSelf(BoundingRegion obj);

		// check collisions with all objects in child nodes
		void checkCollisionsChildren(BoundingRegion obj);

		// destroy object (free memory)
		void destroy();
	};
}