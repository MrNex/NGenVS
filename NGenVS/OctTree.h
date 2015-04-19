#ifndef OCTTREE_H
#define OCTTREE_H

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
static unsigned int defaultOccupancy = 3;
static unsigned int defaultMaxDepth = 4;

//Internal functions

///
//Allocates memory for an octtree node
//
//Returns:
//	A pointer to a newly allocated, uninitialized oct tree node
struct OctTree_Node* OctTree_Node_Allocate();

///
//Initializes an oct tree node to the given specifications
//
//Parameters:
//	node: A pointer to the oct tree node to initialize
//	leftBound: The left bound of the octtree
//	rightBound: The right bound of the octtree
//	bottomBound: The bottom bound of the octtree
//	topBound: The top bound of the octtree
//	backBound: The back bound of the octtree
//	frontBound: The front bound of the octtree
void OctTree_Node_Initialize(struct OctTree_Node* node, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound);

///
//Frees data allocated by an oct tree node
//
//Parameters:
//	node: A pointer to the oct tree node to free
void OctTree_Node_Free(struct OctTree_Node* node);

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

#endif