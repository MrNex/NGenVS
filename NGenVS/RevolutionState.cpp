#include "RevolutionState.h"

#include "TimeManager.h"

struct State_Members
{
	float angularVelocity;
	FrameOfReference* frameOfRevolution;
	Vector* startPoint;
	Vector* axisOfRevolution;
};


///
//Initializes a revolution state
//
//Parameters:
//	s: The state to be initialized as a revolution state
//	startPoint: The point which the body will begin revolving around the pointOfRevolution
//	pointOfRevolution: The point which this state will revolve it's object around
//	axisOfRevolution: The vector representing the axis to revolve around (unit vector please)
//	aVel: The angular velocity which this state will revolve it's object at
void State_Revolution_Initialize(State* s, const Vector* startPoint, Vector* pointOfRevolution, Vector* axisOfRevolution, const float aVel)
{
	s->members = (struct State_Members*)malloc(sizeof(struct State_Members));
	
	s->members->frameOfRevolution = FrameOfReference_Allocate();
	s->members->frameOfRevolution->position = pointOfRevolution;

	s->members->frameOfRevolution->rotation = Matrix_Allocate();
	Matrix_Initialize(s->members->frameOfRevolution->rotation, 3, 3);

	s->members->frameOfRevolution->scale = Matrix_Allocate();
	Matrix_Initialize(s->members->frameOfRevolution->scale, 3, 3);

	s->members->startPoint = Vector_Allocate();
	Vector_Initialize(s->members->startPoint, 3);
	Vector_Copy(s->members->startPoint, startPoint);

	s->members->axisOfRevolution = Vector_Allocate();
	Vector_Initialize(s->members->axisOfRevolution, 3);
	Vector_Copy(s->members->axisOfRevolution, axisOfRevolution);


	s->members->angularVelocity = aVel;

	s->State_Update = State_Revolution_Update;
	s->State_Members_Free = State_Revolution_Free;
}

///
//Frees memory used by a revolution State
//
//Parameters:
//	s: The revolution state to free
void State_Revolution_Free(State* s)
{
	Vector_Free(s->members->startPoint);
	
	Matrix_Free(s->members->frameOfRevolution->rotation);
	Matrix_Free(s->members->frameOfRevolution->scale);

	free(s->members->frameOfRevolution);

	free(s->members);
}

///
//Revolves an object according to it's revolution state
//
//Parameters:
//	GO: The game object with a RevolutionState being revolved
//	state: The revolution State updating the GameObject
void State_Revolution_Update(GObject* GO, State* state)
{
	//Figure out the angle to rotate by
	float dt = TimeManager_GetDeltaSec();
	FrameOfReference_Rotate(state->members->frameOfRevolution, state->members->axisOfRevolution, state->members->angularVelocity * dt);

	Matrix_GetProductVector(GO->frameOfReference->position, state->members->frameOfRevolution->rotation, state->members->startPoint);
	Vector_Increment(GO->frameOfReference->position, state->members->frameOfRevolution->position);
}