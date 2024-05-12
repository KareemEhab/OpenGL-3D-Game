#include "Octree.h"
#include "../graphics/models/Box.hpp"

// calculate bounds of specified quadrant in bounding region
void Octree::calculateBounds(BoundingRegion &out, Octant octant, BoundingRegion parentRegion) 
{
    // find min and max points of corresponding octant

    glm::vec3 center = parentRegion.calculateCenter();
    if (octant == Octant::O1)
        out = BoundingRegion(center, parentRegion.max);
    else if (octant == Octant::O2)
        out = BoundingRegion(glm::vec3(parentRegion.min.x, center.y, center.z), glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z));
    else if (octant == Octant::O3)
        out = BoundingRegion(glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z), glm::vec3(center.x, center.y, parentRegion.max.z));
    else if (octant == Octant::O4)
        out = BoundingRegion(glm::vec3(center.x, parentRegion.min.y, center.z), glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z));
    else if (octant == Octant::O5)
        out = BoundingRegion(glm::vec3(center.x, center.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z));
    else if (octant == Octant::O6)
        out = BoundingRegion(glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z), glm::vec3(center.x, parentRegion.max.y, center.z));
    else if (octant == Octant::O7)
        out = BoundingRegion(parentRegion.min, center);
    else if (octant == Octant::O8)
        out = BoundingRegion(glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, center.y, center.z));
}

// default constructor
Octree::node::node()
    : region(BoundTypes::AABB) {}

// initialize with bounds (no objects yet)
Octree::node::node(BoundingRegion bounds)
    : region(bounds) {}

// initialize with bounds and list of objects
Octree::node::node(BoundingRegion bounds, std::vector<BoundingRegion> objectList)
    : region(bounds) 
{
    objects.insert(objects.end(), objectList.begin(), objectList.end());
}

void Octree::node::addToPending(RigidBody* instance, trie::Trie<Model*> models)
{
    for (BoundingRegion br : models[instance->modelId]->boundingRegions)
    {
        br.instance = instance;
        br.transform();
        queue.push(br);
    }
}

// Build tree (called during initialization)
void Octree::node::build() 
{
    // Variable declarations
    glm::vec3 dimensions = region.calculateDimensions();
    BoundingRegion octants[NO_CHILDREN];
    vector<BoundingRegion> octLists[NO_CHILDREN]; // Array of lists of objects in each octant
    /*
        termination conditions (don't subdivide further)
        - 1 or less objects (ie an empty leaf node or node with 1 object)
        - dimesnions are too small
    */

    // <= 1 objects
    if (objects.size() <= 1)
    {
        goto setVars;
    }

    // Too small
    
    for (int i = 0; i < 3; i++) 
    {
        if (dimensions[i] < MIN_BOUNDS)
        {
            goto setVars;
        }
    }

    // Create regions
    for (int i = 0; i < NO_CHILDREN; i++) 
        calculateBounds(octants[i], (Octant)(1 << i), region);

    // Determine which octants to place objects in
    for (int i = 0, length = objects.size(); i < length; i++) 
    {
        BoundingRegion br = objects[i];
        for (int j = 0; j < NO_CHILDREN; j++) 
        {
            if (octants[j].containsRegion(br)) 
            {
                // Octant contains region
                octLists[j].push_back(br);
                objects.erase(objects.begin() + i);
                i--;
                length--;
                break;
            }
        }
    }

    // Populate octants
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octLists[i].size() != 0) 
        {
            // If children go into this octant
            children[i] = new node(octants[i], octLists[i]);
            States::activateIndex(&activeOctants, i); // activate octant
            children[i]->parent = this;
            children[i]->build();
            hasChildren = true;
        }
    }

setVars:
    // Set state variables
    treeBuilt = true;
    treeReady = true;

    for (int i = 0; i < objects.size(); i++)
        objects[i].cell = this;
}

// Update objects in tree (called during each iteration of main loop)
void Octree::node::update(Box &box)
{
    if (treeBuilt && treeReady) 
    {
        box.positions.push_back(region.calculateCenter());
        box.sizes.push_back(region.calculateDimensions());

        // Countdown timer
        if (objects.size() == 0)
        {
            if (!hasChildren)
            {
                // Ensure no child leaves
                if (currentLifespan == -1)
                    currentLifespan = maxLifespan;
                else if (currentLifespan > 0)
                    currentLifespan--;
            }
        }
        else
        {
            if (currentLifespan != -1)
            {
                if (maxLifespan <= 64)
                    maxLifespan <<= 2; // Multiply by 2
            }
        }

        // Remove objects that don't exist anymore
        for (int i = 0, listSize = objects.size(); i < listSize; i++)
        {
            // Remove if on list of dead objects
            if (States::isActive(&objects[i].instance->state, INSTANCE_DEAD))
            {
                objects.erase(objects.begin() + i);
                // Update counter/size accordingly
                i--;
                listSize--;
            }
        }

        // Get moved objects that were in this leaf in previous frame
        stack<int> movedObjects;
        for (int i = 0, listSize = objects.size(); i < listSize; i++)
        {
            if (States::isActive(&objects[i].instance->state, INSTANCE_MOVED))
            {
                objects[i].transform();
                movedObjects.push(i);
            }
            box.positions.push_back(objects[i].calculateCenter());
            box.sizes.push_back(objects[i].calculateDimensions());
        }

        // Remove dead branches
        unsigned char flags = activeOctants;
        for (int i = 0; flags > 0; flags >>= 1, i++)
        {
            if (States::isIndexActive(&flags, 0) && children[i]->currentLifespan == 0)
            {
                // Active and run out of time
                if (children[i]->objects.size() > 0)
                    children[i]->currentLifespan = -1;
                else // Branch is dead
                {
                    children[i] = nullptr;
                    States::deactivateIndex(&activeOctants, i);
                }
            }
        }

        // Update child nodes
        if (children != nullptr) 
        {
            // Go through each octant using flags
            for (unsigned char flags = activeOctants, i = 0; flags > 0; flags >>= 1, i++) 
            {
                if (States::isIndexActive(&flags, 0)) // Active octant
                    if (children[i] != nullptr)
                        children[i]->update(box);
            }
        }

        // Move moved objects into new nodes
        BoundingRegion movedObj; // placeholder
        while (movedObjects.size() != 0) 
        {
            /*
                For each moved object
                - Traverse up tree (start with current node) until find a node that completely encloses the object
                - Call insert (push object as far down as possible)
            */

            movedObj = objects[movedObjects.top()]; // Set to top object in stack
            node* current = this; // Placeholder

            while (!current->region.containsRegion(movedObj)) 
            {
                // Set current to current's parent (recursion)
                if (current->parent != nullptr)
                    current = current->parent;
                else
                    break; // If not root node
            }

            // When finished
            // Remove from objects list
            // Remove from movedObjects stack
            // Insert into found region
            objects.erase(objects.begin() + movedObjects.top());
            movedObjects.pop();
            current->insert(movedObj);
            

            // Collision detection
            // itself
            current = movedObj.cell;
            current->checkCollisionsSelf(movedObj);

            // children
            current->checkCollisionsChildren(movedObj);

            // parents
            while (current->parent) 
            {
                current = current->parent;
                current->checkCollisionsSelf(movedObj);
            }
        }
    }
    else 
    {
        // Process pending results
        if (queue.size() > 0)
            processPending();
    }
}

// Process pending queue
void Octree::node::processPending() 
{
    if (!treeBuilt) 
    {
        // Add objects to be sorted into branches when built
        while (queue.size() != 0) 
        {
            objects.push_back(queue.front());
            queue.pop();
        }
        build();
    }
    else {
        for (int i = 0, length = queue.size(); i < length; i++)
        {
            BoundingRegion br = queue.front();
            if (region.containsRegion(br)) 
                insert(br); // Insert object immediately
            else
            {
                br.transform();
                queue.push(br);
            }
            queue.pop();
        }
    }
}

// Dynamically insert object into node
bool Octree::node::insert(BoundingRegion obj) 
{
    /*
        Termination conditions
        - No objects (an empty leaf node)
        - Dimensions are less than MIN_BOUNDS
    */

    glm::vec3 dimensions = region.calculateDimensions();
    if (objects.size() == 0 ||
        dimensions.x < MIN_BOUNDS ||
        dimensions.y < MIN_BOUNDS ||
        dimensions.z < MIN_BOUNDS
        ) 
    {
        obj.cell = this;
        objects.push_back(obj);
        return true;
    }

    // Safeguard if object doesn't fit
    if (!region.containsRegion(obj)) 
        return parent == nullptr ? false : parent->insert(obj);

    // Create regions if not defined
    BoundingRegion octants[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; i++) 
    {
        if (children[i] != nullptr) // Child exists, so take its region  
            octants[i] = children[i]->region;
        else // Get region for this octant  
            calculateBounds(octants[i], (Octant)(1 << i), region);
    }

    objects.push_back(obj);

    // Determine which octants to put objects in
    vector<BoundingRegion> octLists[NO_CHILDREN]; // Array of list of objects in each octant
    for (int i = 0, len = objects.size(); i < len; i++) 
    {
        objects[i].cell = this;
        for (int j = 0; j < NO_CHILDREN; j++) 
        {
            if (octants[j].containsRegion(objects[i])) 
            {
                octLists[j].push_back(objects[i]);
                // Remove from objects list
                objects.erase(objects.begin() + i);
                i--;
                len--;
                break;
            }
        }
    }

    // Populate octants
    for (int i = 0; i < NO_CHILDREN; i++) 
    {
        if (octLists[i].size() != 0) 
        {
            // Objects exist in this octant
            if (children[i]) 
            {
                for (BoundingRegion br : octLists[i])
                    children[i]->insert(br);
            }
            else 
            {
                // Create new node
                children[i] = new node(octants[i], octLists[i]);
                children[i]->parent = this;
                States::activateIndex(&activeOctants, i);
                children[i]->build();
                hasChildren = true;
            }
        }
    }

    return true;
}

// Check collisions with all objects in node
void Octree::node::checkCollisionsSelf(BoundingRegion obj) 
{
    for (BoundingRegion br : objects) 
    {
        if (br.intersectsWith(obj)) 
        {
            if (br.instance->instanceId != obj.instance->instanceId) 
            {
                // Different instances collide
                cout << "Instance " << br.instance->instanceId << "(" << br.instance->modelId << ") collides with " << obj.instance->instanceId << "(" << obj.instance->modelId << ")" << std::endl;
                br.intersectsWith(obj);
            }
        }
    }
}

// Check collisions with all objects in child nodes
void Octree::node::checkCollisionsChildren(BoundingRegion obj) 
{
    if (children) 
    {
        for (int flags = activeOctants, i = 0;
            flags > 0;
            flags >>= 1, i++) {
            if (States::isIndexActive(&flags, 0) && children[i]) 
            {
                children[i]->checkCollisionsSelf(obj);
                children[i]->checkCollisionsChildren(obj);
            }
        }
    }
}

// Destroy object (free memory)
void Octree::node::destroy() {
    // clearing out children
    if (children != nullptr) {
        for (int flags = activeOctants, i = 0;
            flags > 0;
            flags >>= 1, i++) {
            if (States::isIndexActive(&flags, 0)) {
                // active
                if (children[i] != nullptr) {
                    children[i]->destroy();
                    children[i] = nullptr;
                }
            }
        }
    }

    // clear this node
    objects.clear();
    while (queue.size() != 0) {
        queue.pop();
    }
}