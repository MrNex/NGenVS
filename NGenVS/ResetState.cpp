#include "ResetState.h"

#include "TimeManager.h"
#include "ObjectManager.h"

struct State_Reset_Members
{
	float currentTime;
	float resetTime;
	Vector* initialPosition;
	Vector* initialImpulse;
};

///
//Initializes a Reset state
//
//Parameters:
//	s: The state to initialize as a Reset state
//	seconds: The number of seconds until removal of this object from the simulation
void State_Reset_Initialize(State* state, float seconds, Vector* initPos, Vector* initImp)
{
	state->members = (State_Members)malloc(sizeof(struct State_Reset_Members));
	//Get members
	struct State_Reset_Members* members = (struct State_Reset_Members*)state->members;

	members->currentTime = 0.0f;
	members->resetTime = seconds;

	members->initialPosition = Vector_Allocate();
	Vector_Initialize(members->initialPosition, 3);
	members->initialImpulse = Vector_Allocate();
	Vector_Initialize(members->initialImpulse, 3);

	Vector_Copy(members->initialPosition, initPos);
	Vector_Copy(members->initialImpulse, initImp);

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

	if(memcmp(members->initialPosition->components, GO->frameOfReference->position->components, sizeof(float)*members->initialPosition->dimension) != 0)
	{
		members->currentTime += TimeManager_GetDeltaSec();
		if(members->currentTime > members->resetTime)
		{
			if(GO->body != NULL)
			{
				Vector_Copy(GO->body->velocity, &Vector_ZERO);
				RigidBody_ApplyImpulse(GO->body, members->initialImpulse, &Vector_ZERO);
			}
			GObject_SetPosition(GO, members->initialPosition);
			Matrix initialRotation;
			Matrix_INIT_ON_STACK(initialRotation, 3, 3);
			GObject_SetRotation(GO, &initialRotation);
			members->currentTime = 0.0f;
		}
	}
}