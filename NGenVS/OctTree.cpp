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