#include "OctTree.h"

#include <stdlib.h>
#include <math.h>

///
//Allocates memory for an octtree node
//
//Returns:
//	A pointer to a newly allocated, uninitialized oct tree node
static struct OctTree_Node* OctTree_Node_Allocate()
{
	struct OctTree_Node* node = (struct OctTree_Node*)malloc(sizeof(struct OctTree_Node));
	return node;
}

///
//Initializes an oct tree node to the given specifications
//
//Parameters:
//	node: A pointer to the oct tree node to initialize
//	tree: A pointer to the oct tree this node is part of
//	parent: A pointer to the parent node of this oct tree node
//	depth: The depth from the root node of this node in the oct tree
//	leftBound: The left bound of the octtree
//	rightBound: The right bound of the octtree
//	bottomBound: The bottom bound of the octtree
//	topBound: The top bound of the octtree
//	backBound: The back bound of the octtree
//	frontBound: The front bound of the octtree
static void OctTree_Node_Initialize(struct OctTree_Node* node, OctTree* tree, struct OctTree_Node* parent, unsigned int depth, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound)
{
	//Set relative nodes
	node->children = NULL;
	node->parent = parent;

	//Create data array
	node->data = DynamicArray_Allocate();
	//Set a max capacity of the dynamic array to the max occupancy of the oct tree
	node->data->capacity = tree->maxOccupancy;
	DynamicArray_Initialize(node->data, sizeof(GObject*));

	//Set depth
	node->depth = depth;

	//Set bounds
	node->left = leftBound;
	node->right = rightBound;
	node->bottom = bottomBound;
	node->top = topBound;
	node->back = backBound;
	node->front = frontBound;
}

///
//Frees data allocated by an oct tree node
//
//Parameters:
//	node: A pointer to the oct tree node to free
static void OctTree_Node_Free(struct OctTree_Node* node)
{
	//If this node has children
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			//Free the children!
			OctTree_Node_Free(node->children + i);
		}
		//Free the children container...
		free(node->children);
	}

	//Free the data contained within this tree
	DynamicArray_Free(node->data);

	//Free this node
	free(node);
}

///
//Allocates the children of an oct tree node
//
//Returns:
//	A pointer to an array of 8 newly allocated, uninitialized node children
static struct OctTree_Node* OctTree_Node_AllocateChildren()
{
	struct OctTree_Node* children = (struct OctTree_Node*)malloc(sizeof(struct OctTree_Node) * 8);
	return children;
}

///
//Initializes the children of an oct tree node
//
//Parameters:
//	tree: A pointer to the oct tree the children will be apart of
//	node: A pointer to the node to initialize the children of
static void OctTree_Node_InitializeChildren(OctTree* tree, struct OctTree_Node* parent)
{
	//Get the half width, half depth, and half height of the parent
	float halfWidth = (parent->right - parent->left)/2.0f;
	float halfHeight = (parent->top - parent->bottom)/2.0f;
	float halfDepth = (parent->front - parent->back)/2.0f;

	//Bottom back right octant
	OctTree_Node_Initialize(parent->children, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom, parent->bottom + halfWidth,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Bottom back left octant
	OctTree_Node_Initialize(parent->children + 1, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom, parent->bottom + halfWidth,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Bottom front left octant
	OctTree_Node_Initialize(parent->children + 2, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom, parent->bottom + halfWidth,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

	//Bottom front right octant
	OctTree_Node_Initialize(parent->children + 3, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom, parent->bottom + halfWidth,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

	//Top back right octant
	OctTree_Node_Initialize(parent->children + 4, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom + halfWidth, parent->top,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Top back left octant
	OctTree_Node_Initialize(parent->children + 5, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom + halfWidth, parent->top,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Top front left octant
	OctTree_Node_Initialize(parent->children + 6, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom + halfWidth, parent->top,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

	//Top front right octant
	OctTree_Node_Initialize(parent->children + 7, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom + halfWidth, parent->top,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

}

//Functions

///
//Allocates memory for an octtree
//
//Returns:
//	Pointer to a newly allocated uninitialized oct tree
OctTree* OctTree_Allocate()
{
	//Allocate
	OctTree* tree = (OctTree*)malloc(sizeof(OctTree));

	//Assign default values
	tree->maxDepth = defaultMaxDepth;
	tree->maxOccupancy = defaultMaxOccupancy;

	return tree;
}

///
//Initializes an oct tree and creates a root node with the given dimensions
//
//Parameters:
//	tree: A pointer to the oct tree to initialize
//	leftBound: The left bound of the octtree
//	rightBound: The right bound of the octtree
//	bottomBound: The bottom bound of the octtree
//	topBound: The top bound of the octtree
//	backBound: The back bound of the octtree
//	frontBound: The front bound of the octtree
void OctTree_Initialize(OctTree* tree, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound)
{
	//Allocate root
	tree->root = OctTree_Node_Allocate();
	//Initialize root
	OctTree_Node_Initialize(tree->root, tree, NULL, 0, leftBound, rightBound, bottomBound, topBound, backBound, frontBound);
}

///
//Frees the data allocated by an octtree.
//Does not free any of the data contained within the octtree!
//
//Parameters:
//	tree: A pointer to the octtree to free
void OctTree_Free(OctTree* tree)
{
	//Free the nodes
	OctTree_Node_Free(tree->root);
	//Free the tree!
	free(tree);
}

///
//Adds a game object to the oct tree
//
//Parameters:
//	tree: A pointer to The oct tree to add a game object to
//	obj: A pointer to the game object to add
void OctTree_Add(OctTree* tree, GObject* obj)
{
	//Add the object to the root node
	OctTree_Node_Add(tree, tree->root, obj);
}

///
//Adds a game object to a node of the oct tree
//
//Parameters:
//	tree: THe oct tree to add the object to
//	node: The node to add the object to
//	obj: A pointer to the game object being added to the tree
static void OctTree_Node_Add(OctTree* tree, struct OctTree_Node* node, GObject* obj)
{
	//If this node has children, determine which children the object collides with
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			unsigned char collisionStatus = OctTree_Node_DoesObjectCollide(node, obj);
			//If the object is fully contained in this node
			if(collisionStatus == 2)
			{
				//Add the nobject to this node & stop looping
				OctTree_Node_Add(tree, node->children + i, obj);
				break;
			}
			//Else if the object is partially contained in this node
			else  if(collisionStatus == 1)
			{
				//Add the object to this node & keep looping
				OctTree_Node_Add(tree, node->children + i, obj);
			}
		}
	}
	//If this node has no children
	else
	{
		//Can we hold another object? or are we too deep to subdivide?
		if(node->data->size <= tree->maxOccupancy || node->depth >= tree->maxDepth)
		{
			//Make sure we aren't already holding a pointer to the object...
			if(DynamicArray_ContainsWithin(node->data, &obj, node->data->size) == 0)
			{
				//Add the object!
				DynamicArray_Append(node->data, &obj);
			}
		}
		//Else, we are out of room and can subdivide!
		else
		{
			OctTree_Node_Subdivide(tree, node);
			//Finally, recall this function to add the object to one of the children
			OctTree_Node_Add(tree, node, obj);
		}
	}
}

///
//Subdivides an oct tree node into 8 child nodes, re-adding all occupants to the oct tree
//
//Parameters:
//	tree: A pointer to the oct tree in which this node lives
//	node: A pointer to the node being subdivided
static void OctTree_Node_Subdivide(OctTree* tree, struct OctTree_Node* node)
{
	//Allocate this nodes children
	node->children = OctTree_Node_AllocateChildren();

	//Initialize this nodes children
	OctTree_Node_InitializeChildren(tree, node);
	GObject* current;
	//re-add all contents to the node
	for(int i = 0; i < node->data->size; i++)
	{
		//Get the GObject* at index i
		current = *((GObject**)DynamicArray_Index(node->data, i));
		//Add the GObject* back into the node
		OctTree_Node_Add(tree, node, current);
	}

	//Clear the node's data
	DynamicArray_Clear(node->data);
}

///
//Determines if and how a game object is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	obj: The game object to test for
//
//Returns:
//	0 if the object does not collide with the octent
//	1 if the object intersects the octent but is not contained within the octent
//	2 if the object is completely contained within the octent
static unsigned char OctTree_Node_DoesObjectCollide(OctTree_Node* node, GObject* obj)
{
	unsigned char collisionStatus = 0;
	//Determine which frame of reference we will be using to orient the object
	FrameOfReference* primaryFrame;
	if(obj->body != NULL)
	{
		primaryFrame = obj->body->frame;
	}
	else
	{
		primaryFrame = obj->frameOfReference;
	}

	//Determine the type of collider the object has
	switch(obj->collider->type)
	{
	case COLLIDER_SPHERE:
		collisionStatus = OctTree_Node_DoesSphereCollide(node, obj->collider->data->sphereData, primaryFrame);
		break;
	case COLLIDER_AABB:
		collisionStatus = OctTree_Node_DoesAABBCollide(node, obj->collider->data->AABBData, primaryFrame);
		break;
	case COLLIDER_CONVEXHULL:
		collisionStatus = OctTree_Node_DoesConvexHullCollide(node, obj->collider->data->convexHullData, primaryFrame);
		break;
	}

	return collisionStatus;
}


///
//Determines if and how a sphere collider is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	sphere: The sphere to test for
//	frame: The frame of reference to orient the sphere when checking
//
//Returns:
//	0 if the sphere does not collide with the octent
//	1 if the sphere intersects the octent but is not contained within the octent
//	2 if the sphere is completely contained within the octent
static unsigned char OctTree_Node_DoesSphereCollide(OctTree_Node* node, ColliderData_Sphere* sphere, FrameOfReference* frame)
{
	unsigned char collisionStatus = 0;

	//Get the difference between each bound of the octent and the corresponding bound of the sphere
	float differences[6];	//0 = left, 1 = right
							//2 = bottom, 3 = top
							//4 = back, 5 = front;

	///
	//This is what we are going to do, essentially, but we will do it using a loop and
	//the fact that structs are (w/o memory alignment) contiguous in memory
	//differences[0] = node->left - (frame->position->components[0] - sphere->radius);

	//For reference, get a pointer to where the first bound is located in the node struct
	float* reference = &(node->left);

	for(int i = 0; i < 6; i++)
	{
		if( i % 2 == 0)
			differences[i] = *(reference + i) - (frame->position->components[i / 2] - sphere->radius);
		else
			differences[i] = *(reference + i) - (frame->position->components[i / 2] + sphere->radius);
	}

	//Get the dimensions of the node
	float dimensions[3] = 
	{
		node->right - node->left,		//0 = width
		node->top - node->bottom,		//1 = height
		node->front - node->back		//2 = depth

	};	
	
	//Determine if the bounds overlap
	unsigned char overlap = 1;
	for(int i = 0; i < 6; i++)
	{
		if(fabs(differences[i]) >= dimensions[i/2])
		{
			overlap = 0;
			break;
		}
	}

	//Set the collision status
	collisionStatus = overlap;

	//If we found that the bounds do overlap, we must check if the node contains the sphere
	if(collisionStatus == 1)
	{
		for(int i = 0; i < 6; i++)
		{
			//If checking left, bottom, or back
			if(i % 2 == 0)
			{
				if(differences[i] >= 0)
				{
					overlap = 0;
					break;
				}
			}
			//else checking right, top, or front
			else
			{
				if(differences[i] <= 0)
				{
					overlap = 0;
					break;
				}
			}
		}

		//Update collision status
		collisionStatus += overlap;
	}

	return collisionStatus;
}

///
//Determines if and how an AABB is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	AABB: The AABB to test for
//	frame: The frame of reference with which to orient the AABB
//
//Returns:
//	0 if the AABB does not collide with the octent
//	1 if the AABB intersects the octent but is not contained within the octent
//	2 if the AABB is completely contained within the octent
static unsigned char OctTree_Node_DoesAABBCollide(OctTree_Node* node, ColliderData_AABB* AABB, FrameOfReference* frame)
{
	unsigned char collisionStatus = 0;

	//Get centroid in world space to have the real center of the AABB
	Vector pos;
	Vector_INIT_ON_STACK(pos, 3);
	Vector_Add(&pos, AABB->centroid, frame->position);

	//Get the difference between each bound of the octent and the corresponding bound of the AABB
	float differences[6];	//0 = left, 1 = right
							//2 = bottom, 3 = top
							//4 = back, 5 = front;

	//For reference, get a pointer to where the first bound is located in the node struct and a pointer to the first dimension in the AABBData
	float* boundRef = &(node->left);
	float* dimRef = &(AABB->width);


	for(int i = 0; i < 6; i++)
	{
		//If getting difference of left bottom or back
		if(i % 2 == 0)
			differences[i] = *(boundRef + i) - (pos.components[i / 2] - *(dimRef + (i / 2)));
		//else getting difference of right top or front
		else
			differences[i] = *(boundRef + i) - (pos.components[i / 2] + *(dimRef + (i / 2)));
	}

	//Get the dimensions of the node
	float dimensions[3] = 
	{
		node->right - node->left,		//0 = width
		node->top - node->bottom,		//1 = height
		node->front - node->back		//2 = depth

	};	

	//Determine if the bounds overlap
	unsigned char overlap = 1;
	for(int i = 0; i < 6; i++)
	{
		if(fabs(differences[i]) >= dimensions[i/2])
		{
			overlap = 0;
			break;
		}
	}

	//Set the collision status
	collisionStatus = overlap;

	//If we found that the bounds do overlap, we must check if the node contains the sphere
	if(collisionStatus == 1)
	{
		for(int i = 0; i < 6; i++)
		{
			//If checking left, bottom, or back
			if(i % 2 == 0)
			{
				if(differences[i] >= 0)
				{
					overlap = 0;
					break;
				}
			}
			//else checking right, top, or front
			else
			{
				if(differences[i] <= 0)
				{
					overlap = 0;
					break;
				}
			}
		}

		//Update collision status
		collisionStatus += overlap;
	}

	return collisionStatus;

}

///
//Determines if and how a convex hull is colliding with an oct tree node.
//Uses the minimum AABB as a test, not the convex hull itself.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	convexHull: The convex hull to test for
//	frame: The frame of reference with which to orient the convex hull
//
//Returns:
//	0 if the convexHull does not collide with the octent
//	1 if the convexHull intersects the octent but is not contained within the octent
//	2 if the convexHull is completely contained within the octent
static unsigned char OctTree_Node_DoesConvexHullCollide(OctTree_Node* node, ColliderData_ConvexHull* convexHull, FrameOfReference* frame)
{
	//Generate the minimum AABB from the convex hull
	ColliderData_AABB AABB;
	Vector AABBCentroid;
	Vector_INIT_ON_STACK(AABBCentroid, 3);
	AABB.centroid = &AABBCentroid;

	ConvexHullCollider_GenerateMinimumAABB(&AABB, convexHull, frame);

	//Then use the AABB test
	return OctTree_Node_DoesAABBCollide(node, &AABB, frame);
}