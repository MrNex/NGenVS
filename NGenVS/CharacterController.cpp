#include "CharacterController.h"

#include "InputManager.h"
#include "RenderingManager.h"
#include "TimeManager.h"
#include "PhysicsManager.h"

#include <stdio.h>

// The members that affect the object
// MaxSpeed is not incorporated yet though. (Fyi)
struct State_Members
{
	float rotationSpeed;
	float movementSpeed;
	float maxSpeed;
};

// Initialize the character state

// [Based off of Camera Initialize]
// Initialize the character State
// Param:
//  s: The state of intiialize
void State_CharacterController_Initialize(State* s, float velocity, float angularVelocity)
{
	s->members = (struct State_Members*)malloc(sizeof(struct State_Members));

	s->members->movementSpeed = velocity;
	s->members->rotationSpeed = angularVelocity;
	// setting a base max speed for the moment
	s->members->maxSpeed = 5.0f;

	s->State_Update = State_CharacterController_Update;
	s->State_Members_Free = State_CharacterController_Free;
}

// Frees all of the members in the state
void State_CharacterController_Free(State* s)
{
	free(s->members);
}

// Updates the render manager and moves/rotates the object.
// GO: Game Object that the state is attached to
// state: The first person camera State updating the game object.
void State_CharacterController_Update(GObject* GO, State* state)
{
	State_CharacterController_Rotate(GO,state);
	State_CharacterController_Translate(GO,state);
}

// Rotation for the character controller
// This should need no changes?
void State_CharacterController_Rotate(GObject* GO, State* state)
{
	Camera* cam = RenderingManager_GetRenderingBuffer().camera;

	// if player's mouse is locked
	if(InputManager_GetInputBuffer().mouseLock)
	{
		// Gets the time per second
		float dt = TimeManager_GetDeltaSec();

		int deltaMouseX = (InputManager_GetInputBuffer().mousePosition[0] - InputManager_GetInputBuffer().previousMousePosition[0]);
		int deltaMouseY = (InputManager_GetInputBuffer().mousePosition[1] - InputManager_GetInputBuffer().previousMousePosition[1]);

		Vector* axis = Vector_Allocate();
		Vector_Initialize(axis,3);

		if(deltaMouseX != 0)
		{
			axis->components[1] = 1.0f;
			// rotate the camera
			Camera_Rotate(cam,axis,state->members->rotationSpeed * deltaMouseX);
			axis->components[1] = 0.0f;
		}
		
		if (deltaMouseY != 0)
		{
			axis->components[0] = 1.0f;

			Camera_Rotate(cam, axis, state->members->rotationSpeed * deltaMouseY);
			axis->components[0] = 0.0f;
		}

		Vector_Free(axis);

	}
}

// translate the character and his bounding box.
// Parameters:
//   GO: The game object this state is attached to (Used for translating the bounding box)
//   state: The first person camera state updating the gameObject
void State_CharacterController_Translate(GObject* GO, State* state)
{
	Camera* cam = RenderingManager_GetRenderingBuffer().camera;

	if(InputManager_GetInputBuffer().mouseLock)
	{
		Vector netMvmtVec;
		Vector partialMvmtVec;
		Vector_INIT_ON_STACK(netMvmtVec, 3);
		Vector_INIT_ON_STACK(partialMvmtVec, 3);


		if (InputManager_IsKeyDown('w'))
		{
			//Get "back" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 2, 0, 3);
			//Subtract "back" Vector from netMvmtVec
			Vector_Decrement(&netMvmtVec, &partialMvmtVec);
			//Or in one step but less pretty... Faster though. I think I want readable here for now though.
			//Vector_DecrementArray(netMvmtVec.components, Matrix_Index(cam->rotationMatrix, 2, 0), 3);
		}
		if (InputManager_IsKeyDown('s'))
		{
			//Get "back" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 2, 0, 3);
			//Add "back" Vector to netMvmtVec
			Vector_Increment(&netMvmtVec, &partialMvmtVec);
		}
		if (InputManager_IsKeyDown('a'))
		{
			//Get "Right" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 0, 0, 3);
			//Subtract "Right" Vector From netMvmtVec
			Vector_Decrement(&netMvmtVec, &partialMvmtVec);
		}
		if (InputManager_IsKeyDown('d'))
		{
			//Get "Right" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 0, 0, 3);
			//Add "Right" Vector to netMvmtVec
			Vector_Increment(&netMvmtVec, &partialMvmtVec);
		}

		float dt = TimeManager_GetDeltaSec();

		if (Vector_GetMag(&netMvmtVec) > 0.0f && dt > 0.0f)
		{
			Vector_Normalize(&netMvmtVec);
			Vector_Scale(&netMvmtVec, state->members->movementSpeed * dt);

			GObject_Translate(GO,&netMvmtVec);
			Camera_SetPosition(cam,GO->frameOfReference->position);
		}
	}
}