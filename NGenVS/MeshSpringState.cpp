#include "MeshSpringState.h"

#include "TimeManager.h"
#include "InputManager.h"

#include "DynamicArray.h"

#include <stdio.h>
#include <math.h>


struct MeshSpringState_Node
{
	Vector vertex;
	Vector* velocity;

	Vector* neighbors[6];
	unsigned int numNeighbors;

	unsigned char isAnchor;

};

struct State_Members
{
	struct MeshSpringState_Node* nodes;
	unsigned int numNodes;

	unsigned int gridWidth;
	unsigned int gridHeight;
	unsigned int gridDepth;

	float springConstant;
	float dampingCoefficient;
};



///
//Initializes a mesh spring state
//
//Parameters:
//	state: The state to initialize
//	mesh: The grid mesh whose vertices are acting like springs
//	gridWidth: The number of vertices along the width of the grid
//	gridHeight: The number of vertices along the height of the grid
//	gridDepth: The number of vertices along the depth of the grid
//	springConstant: The value of the spring constant to use in the spring simulation between vertices
//	dampingCoefficient: The value of the damping coefficient to use int ehs pring simulation between vertices
void State_MeshSpringState_Initialize(State* state, Mesh* grid, unsigned int gridWidth, unsigned int gridHeight, unsigned int gridDepth, float springConstant, float dampingCoefficient, int anchorDimensions)
{
	state->State_Members_Free = State_MeshSpringState_Free;
	state->State_Update = State_MeshSpringState_Update;

	state->members = (struct State_Members*)malloc(sizeof(struct State_Members));

	//Assign grid dimensions
	state->members->gridWidth = gridWidth;
	state->members->gridHeight = gridHeight;
	state->members->gridDepth = gridDepth;

	//TODO: Remove hardcoding
	//Set spring constant
	state->members->springConstant = springConstant;
	state->members->dampingCoefficient = dampingCoefficient;

	//Create array of nodes
	state->members->nodes = (struct MeshSpringState_Node*)malloc(sizeof(struct MeshSpringState_Node) * gridWidth * gridHeight * gridDepth);
	state->members->numNodes = 0;


	int anchors = 0;

	//Create each node in the grid
	for(int k = 0; k < gridDepth; k++)
	{
		for(int j = 0; j < gridHeight; j++)
		{
			for(int i = 0; i < gridWidth; i++)
			{
				unsigned int nodeIndex = i + j * gridWidth + k * gridWidth * gridHeight;
				state->members->nodes[nodeIndex].vertex.dimension = 3;
				state->members->nodes[nodeIndex].vertex.components = (float*)(((Vertex*)grid->triangles) + nodeIndex);

				state->members->numNodes++;

				//Check if node is on an edge
				int isBounds = 0;
				if(i == 0 || i == gridWidth - 1) isBounds++;
				if(j == 0 || j == gridHeight - 1) isBounds++;
				if(k == 0 || k == gridDepth - 1) isBounds++;

				//If so, anchor it!
				if(isBounds > 3 - anchorDimensions)
				{
					anchors++;
					state->members->nodes[nodeIndex].isAnchor = 1;
				}
				//Else, give it a velocity
				else
				{
					state->members->nodes[nodeIndex].isAnchor = 0;
					state->members->nodes[nodeIndex].velocity = Vector_Allocate();
					Vector_Initialize(state->members->nodes[nodeIndex].velocity, 3);

				}



				//initialize neighbors to 0
				for(int n = 0; n < 6; n++)
				{
					state->members->nodes[nodeIndex].neighbors[n] = 0;
				}
			}
		}

		printf("Number of nodes:\t%d\nNumber of anchors:\t%d\n", state->members->numNodes, anchors);
	}


	//Connect node to neighbor nodes
	for(int k = 0; k < gridDepth; k++)
	{
		for(int j = 0; j < gridHeight; j++)
		{
			for(int i = 0; i < gridWidth; i++)
			{
				unsigned int nodeIndex = i + j * gridWidth + k * gridWidth * gridHeight;

				//If this node is not an anchor
				if(!state->members->nodes[nodeIndex].isAnchor)
				{
					int numNeighbors = 0;
					//Can it have a left neighbor?
					if(i > 0)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = (i-1) + j * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = state->members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						state->members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a right neighbor?
					if(i < gridWidth - 1)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = (i+1) + j * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = state->members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						state->members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a bottom neighbor?
					if(j > 0)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + (j-1) * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = state->members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						state->members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a top neighbor?
					if(j < gridHeight - 1)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + (j+1) * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = state->members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						state->members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a back neighbor?
					if(k > 0)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + j * gridWidth + (k-1) * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = state->members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						state->members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a front neighbor?
					if(k < gridDepth - 1)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + j * gridWidth + (k+1) * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = state->members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						state->members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					state->members->nodes[nodeIndex].numNeighbors = numNeighbors;
				}
			}
		}
	}
}

///
//Frees resources allocated by a Mesh Spring state
//
//Parameters:
//	s: The state to free
void State_MeshSpringState_Free(State* state)
{
	//For each node
	for(int i = 0; i < state->members->numNodes; i++)
	{
		//Free the nodes velocity
		Vector_Free(state->members->nodes[i].velocity);
	}
	//Free the list of nodes
	free(state->members->nodes);

	//Free the state's members
	free(state->members);
}

///
//Updates the mesh spring state
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The state updating this gameObject
void State_MeshSpringState_Update(GObject* GO, State* state)
{
	//Get the change in second
	float dt = TimeManager_GetDeltaSec();

	//For each node
	for(int i = 0; i < state->members->numNodes; i++)
	{
		unsigned int nodeIndex = i;
		struct MeshSpringState_Node* current = state->members->nodes + nodeIndex;

		int middleNodeIndex = (int)state->members->gridWidth /2 + (int)state->members->gridWidth * (int)state->members->gridHeight/2;

		//If the node is not an anchor
		if(!current->isAnchor)
		{

			//Compute the net force the neighbors are having upon it
			Vector netForce;
			Vector_INIT_ON_STACK(netForce, 3);

			Vector currForce;
			Vector_INIT_ON_STACK(currForce, 3);

			for(int j = 0; j < current->numNeighbors; j++)
			{
				//Get current force from this neighbor
				Vector_Subtract(&currForce, &current->vertex, current->neighbors[j]);

				Vector_Scale(&currForce, state->members->springConstant * (-1.0f));



				//Increment net force by curr force
				Vector_Increment(&netForce, &currForce);
			}

			//Apply damping force
			Vector_GetScalarProduct(&currForce, current->velocity, -state->members->dampingCoefficient);
			//Increment net force by damping force
			Vector_Increment(&netForce, &currForce);

			//If the current node is in the bottom row && it is not an anchor
			if(i > state->members->gridWidth && i < state->members->gridWidth * 2)
			{
				if(!current->isAnchor)
				{
					//If spacebar is pressed
					if(InputManager_IsKeyDown('k'))
					{

						//Apply a force on the positive Z axis
						Vector_GetScalarProduct(&currForce, &Vector_E3, 5.0f);

						Vector_Increment(&netForce, &currForce);
					}
				}
			}
			
			//If the current node is a central node
			if(i == middleNodeIndex || i + 1 == middleNodeIndex || i - 1 == middleNodeIndex || 
				i + state->members->gridWidth == middleNodeIndex || i - state->members->gridWidth == middleNodeIndex)
			{
				if(!current->isAnchor)
				{
					//If spacebar is pressed
					if(InputManager_IsKeyDown('i'))
					{

						//Apply a force on the positive Z axis
						Vector_GetScalarProduct(&currForce, &Vector_E3, 10.0f);

						Vector_Increment(&netForce, &currForce);
					}
				}
			}

			//If the current node is on the back face
			if (i < state->members->gridWidth * state->members->gridHeight)
			{
				if (!current->isAnchor)
				{
					//If spacebar is pressed
					if (InputManager_IsKeyDown('j'))
					{

						//Apply a force on the positive Z axis
						Vector_GetScalarProduct(&currForce, &Vector_E3, -10.0f);

						Vector_Increment(&netForce, &currForce);
					}
				}

			}
			//Acceleration = Force because mass is negligible
			//Find velocity using V = AT
			Vector_Scale(&netForce, dt);
			//Increment velocity
			Vector_Increment(current->velocity, &netForce);

			//Find dX = VT
			Vector_GetScalarProduct(&netForce, current->velocity, dt);

			//Increment position
			Vector_Increment(&current->vertex, &netForce);



		}
	}
}
