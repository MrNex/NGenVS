#include "RemoveState.h"

#include "TimeManager.h"
#include "ObjectManager.h"

static struct State_Members
{
	float currentTime;
	float removeTime;
};

///
//Initializes a remove state
//
//Parameters:
//	s: The state to initialize as a remove state
//	seconds: The number of seconds until removal of this object from the simulation
void State_Remove_Initialize(State* state, float seconds)
{
	state->members = (struct State_Members*)malloc(sizeof(struct State_Members));
	state->members->currentTime = 0.0f;
	state->members->removeTime = seconds;

	state->State_Members_Free = State_Remove_Free;
	state->State_Update = State_Remove_Update;
}

///
//Frees memory allocated by a remove state
//
//Parameters:
//	state: The state to free
void State_Remove_Free(State* state)
{
	free(state->members);
}

///
//Updates a remove state removing the object when the timer is up
//
//Parameters:
//	GO: The game object with the attached remove state
//	state: THe state being updated
void State_Remove_Update(GObject* GO, State* state)
{
	state->members->currentTime += TimeManager_GetDeltaSec();
	if(state->members->currentTime > state->members->removeTime)
	{
		ObjectManager_RemoveObject(GO);
	}
}