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
	unsigned int depthFromRoot;

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

#endif