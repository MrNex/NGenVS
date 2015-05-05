#include "ScoreState.h"
#include <stdio.h>

#include "CollisionManager.h"

struct State_Score_Members
{
	int worth;
};

///
//Initializes a new score state
//
//Parameters:
//	state: The state to initialize as a score state
//	increment: The amount of points to add per hit
void State_Score_Initialize(State* state, const int increment)
{
	struct State_Score_Members* members = (struct State_Score_Members*)malloc(sizeof(struct State_Score_Members));
	state->members = members;

	members->worth = increment;

	state->State_Members_Free = State_Score_Free;
	state->State_Update = State_Score_Update;
}

///
//Frees resources allocated by a score state
//
//Parameters:
//	state: A pointer to the state to free
void State_Score_Free(State* state)
{
	struct State_Score_Members* members = (struct State_Score_Members*)state->members;
	free(members);
}

///
//Checks if the attached object was in a collision with a bullet, then increments & prints the score if so
//
//parameters:
//	GO: a pointer to The game object with an attached score state being updated
//	state: a pointer to the score state updating the attached game object
void State_Score_Update(GObject* GO, State* state)
{
	if(GO->collider->currentCollisions->size > 0)
	{
		//Loop through the collisions which occurred previous frame
		LinkedList_Node* current = GO->collider->currentCollisions->head;
		Collision* currentCollision;
		while(current != NULL)
		{
			//Check if any of the objects involved in the collision are a bullet
			//TODO: MAke a tagging system so this doesn't need to happen
			currentCollision = (Collision*)current->data;
			//Bullets are the only thing with a scale of 0.3
			if(currentCollision->obj1->frameOfReference->scale->components[0] == 0.3f || currentCollision->obj2->frameOfReference->scale->components[0] == 0.3f)
			{
				struct State_Score_Members* members = (struct State_Score_Members*)state->members;
				score += members->worth;
				//Print the new score
				printf("Score:\t%d\n", score);
			}
			current = current->next;
		}
	}
}