#include "OctTree.h"

#include <stdlib.h>

#include "GObject.h"	//The data the oct tree will contain

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