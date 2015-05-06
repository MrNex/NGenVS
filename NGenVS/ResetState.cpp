#include "ResetState.h"

#include <stdio.h>

#include "TimeManager.h"
#include "ObjectManager.h"

struct State_Reset_Members
{
	float currentTime;
	float resetTime;
	float resetDistance;
	Vector* initialPosition;
	Vector* initialImpulse;
	Matrix* initialRotation;
};

///
//Initializes a Reset state
//
//Parameters:
//	s: The state to initialize as a Reset state
//	seconds: The number of seconds until removal of this object from the simulation
void State_Reset_Initialize(State* state, float seconds, float distance, Vector* initPos, Vector* initImp, Matrix* initRot)
{
	state->members = (State_Members)malloc(sizeof(struct State_Reset_Members));
	//Get members
	struct State_Reset_Members* members = (struct State_Reset_Members*)state->members;

	members->currentTime = 0.0f;
	members->resetTime = seconds;

	members->resetDistance = distance;

	members->initialPosition = Vector_Allocate();
	Vector_Initialize(members->initialPosition, 3);
	members->initialImpulse = Vector_Allocate();
	Vector_Initialize(members->initialImpulse, 3);
	

	Vector_Copy(members->initialPosition, initPos);
	Vector_Copy(members->initialImpulse, initImp);

	members->initialRotation = Matrix_Allocate();
	Matrix_Initialize(members->initialRotation, 3, 3);

	Matrix_Copy(members->initialRotation, initRot);

	state->State_Members_Free = State_Reset_Free;
	state->State_Update = State_Reset_Update;
}

///
//Frees memory allocated by a Reset state
//
//Parameters:
//	state: The state to free
void State_Reset_Free(State* state)
{
	//Get members
	struct State_Reset_Members* members = (struct State_Reset_Members*)state->members;
	Vector_Free(members->initialPosition);
	Vector_Free(members->initialImpulse);
	Matrix_Free(members->initialRotation);

	free(members);
}

///
//Updates a Reset state removing the object when the timer is up
//
//Parameters:
//	GO: The game object with the attached Reset state
//	state: THe state being updated
void State_Reset_Update(GObject* GO, State* state)
{
	//Get members
	struct State_Reset_Members* members = (struct State_Reset_Members*)state->members;

	Vector diff;
	Vector_INIT_ON_STACK(diff, 3);
	Vector_Subtract(&diff, members->initialPosition, GO->frameOfReference->position);
	if(Vector_GetMag(&diff) > members->resetDistance)
	{
		members->currentTime += TimeManager_GetDeltaSec();
		if(members->currentTime > members->resetTime)
		{
			printf("Resetting\n");
			if(GO->body != NULL)
			{
				Vector_Copy(GO->body->velocity, &Vector_ZERO);
				Vector_Copy(GO->body->angularVelocity, &Vector_ZERO);
				RigidBody_ApplyImpulse(GO->body, members->initialImpulse, &Vector_ZERO);
			}
			GObject_SetPosition(GO, members->initialPosition);

			GObject_SetRotation(GO, members->initialRotation);
			members->currentTime = 0.0f;
		}
	}
}