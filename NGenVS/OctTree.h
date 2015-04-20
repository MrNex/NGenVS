#ifndef OCTTREE_H
#define OCTTREE_H

#include "GObject.h"		//The data the oct tree will contain
#include "DynamicArray.h"

struct OctTree_Node
{
	//Pointer to the parent of this node
	struct OctTree_Node* parent;
	//Pointer to array of children of this node
	struct OctTree_Node* children;
	
	//The data contained in this node
	DynamicArray* data;

	//The depth of this node from the root of the tree
	//The root has a depth of 0.
	unsigned int depth;

	//Bounds of this oct tree node
	//WARNING:
	//IF YOU CHANGE THE ORDER OF THESE REWRITE OCTTREE_NODE_DOESOBJECTCOLLIDE
	float left, right;		//Width
	float bottom, top;		//Height
	float back, front;		//Depth
};

typedef struct OctTree
{
	//Pointer to the root of the tree
	OctTree_Node* root;

	//Attributes of the tree
	unsigned int maxDepth;		//How many subdivisions can exist
	unsigned int maxOccupancy;	//How many occupants can an octtree have before trying to subdivide
								//This number will be exceeded if maxDepth is reached.
} OctTree;

//Internal members
static unsigned int defaultMaxOccupancy = 3;
static unsigned int defaultMaxDepth = 4;

//Internal functions

///
//Allocates memory for an octtree node
//
//Returns:
//	A pointer to a newly allocated, uninitialized oct tree node
static struct OctTree_Node* OctTree_Node_Allocate();

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
static void OctTree_Node_Initialize(struct OctTree_Node* node, OctTree* tree,  struct OctTree_Node* parent, unsigned int depth, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound);

///
//Frees data allocated by an oct tree node
//
//Parameters:
//	node: A pointer to the oct tree node to free
static void OctTree_Node_Free(struct OctTree_Node* node);

///
//Allocates the children of an oct tree node
//
//Returns:
//	A pointer to an array of 8 newly allocated, uninitialized node children
static struct OctTree_Node* OctTree_Node_AllocateChildren();

///
//Initializes the children of an oct tree node
//
//Parameters:
//	tree: A pointer to the oct tree the children will be apart of
//	node: A pointer to the node to initialize the children of
static void OctTree_Node_InitializeChildren(OctTree* tree, struct OctTree_Node* parent);

//Functions

///
//Allocates memory for an octtree
//
//Returns:
//	Pointer to a newly allocated uninitialized oct tree
OctTree* OctTree_Allocate();

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
void OctTree_Initialize(OctTree* tree, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound);

///
//Frees the data allocated by an octtree.
//Does not free any of the data contained within the octtree!
//
//Parameters:
//	tree: A pointer to the octtree to free
void OctTree_Free(OctTree* tree);

///
//Adds a game object to the oct tree
//
//Parameters:
//	tree: A pointer to The oct tree to add a game object to
//	obj: A pointer to the game object to add
void OctTree_Add(OctTree* tree, GObject* obj);

///
//Adds a game object to a node of the oct tree
//
//Parameters:
//	tree: THe oct tree to add the object to
//	node: The node to add the object to
//	obj: A pointer to the game object being added to the tree
static void OctTree_Node_Add(OctTree* tree, struct OctTree_Node* node, GObject* obj);

///
//Subdivides an oct tree node into 8 child nodes, re-adding all occupants to the oct tree
//
//Parameters:
//	tree: A pointer to the oct tree in which this node lives
//	node: A pointer to the node being subdivided
static void OctTree_Node_Subdivide(OctTree* tree, struct OctTree_Node* node);

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
static unsigned char OctTree_Node_DoesObjectCollide(OctTree_Node* node, GObject* obj);

///
//Determines if and how a sphere collider is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	sphere: The sphere to test for
//	frame: The frame of reference with which to orient the sphere
//
//Returns:
//	0 if the sphere does not collide with the octent
//	1 if the sphere intersects the octent but is not contained within the octent
//	2 if the sphere is completely contained within the octent
static unsigned char OctTree_Node_DoesSphereCollide(OctTree_Node* node, ColliderData_Sphere* sphere, FrameOfReference* frame);

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
static unsigned char OctTree_Node_DoesAABBCollide(OctTree_Node* node, ColliderData_AABB* AABB, FrameOfReference* frame);

///
//Determines if and how a convex hull is colliding with an oct tree node.
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
static unsigned char OctTree_Node_DoesConvexHullCollide(OctTree_Node* node, ColliderData_ConvexHull* convexHull, FrameOfReference* frame);

#endif