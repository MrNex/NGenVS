#include "SpringState.h"
#include <stdio.h>

struct State_Members
{
	Vector* restPosition;
	float k;
};

///
//Initializes a new spring state
//Any GObject with this state attached must have a rigidbody!!
//
//Parameters:
//	s: State to initialize
//	k: Spring constant (How stiff is the spring, (0.0f - 1.0f))
//	restPosition: Position where spring sits at rest
void State_Spring_Initialize(State* s, const float k, const Vector* restPosition)
{
	s->members = (struct State_Members*)malloc(sizeof(struct State_Members));
	
	s->members->restPosition = Vector_Allocate();
	Vector_Initialize(s->members->restPosition, 3);
	Vector_Copy(s->members->restPosition, restPosition);

	s->members->k = k;

	s->State_Members_Free = State_Spring_Free;
	s->State_Update = State_Spring_Update;
}

///
//Frees resources allocated by a spring state
//Call State_Free! Not this! State_Free will call this.
//
//Parameters:
//	s: The spring state to free
void State_Spring_Free(State* s)
{
	Vector_Free(s->members->restPosition);
	free(s->members);
}

///
//Applies forces to an object's Rigidbody component to 
//simulate spring mechanics.
//
//Parameters:
//	GO: Game Object with a SpringState being updated.
//	state: The springstate updating the game object
void State_Spring_Update(GObject* GO, State* state)
{
	Vector dx;
	Vector_INIT_ON_STACK(dx, 3);

	Vector_Subtract(&dx, GO->body->frame->position, state->members->restPosition);
	Vector_Scale(&dx, -state->members->k);

	RigidBody_ApplyForce(GO->body, &dx, &Vector_ZERO);

}