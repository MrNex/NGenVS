#ifndef STATE_RESET_H
#define STATE_RESET_H

#include "State.h"
#include "GObject.h"

///
//Initializes a reset state
//
//Parameters:
//	state: The state to initialize as a remove state
//	seconds: The number of seconds until reset of this object in the simulation
void State_Remove_Initialize(State* state, float seconds, Vector* initPos, Vector* initImp);

///
//Frees memory allocated by a reset state
//
//Parameters:
//	state: The state to free
void State_Remove_Free(State* state);

///
//Updates a reset state, resettig the object when the timer is up
//
//Parameters:
//	GO: The game object with the attached reset state
//	state: THe state being updated
void State_Remove_Update(GObject* GO, State* state);

#endif