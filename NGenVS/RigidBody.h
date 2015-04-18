#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "DynamicArray.h"
#include "FrameOfReference.h"

typedef struct RigidBody
{
	float coefficientOfRestitution;		//How elastic will this float act in a collision (0.0f - 1.0f)
	float inverseMass;					//Because schwartz
	Matrix* inverseInertia;				//inverse moment of inertia matrix
	Matrix* inertia;					//Moment of inertia matrix
	Vector* netForce;					//Total net force this instant
	Vector* previousNetForce;			//Total net force previous instant
	Vector* netImpulse;					//net impulses this instant
	Vector* netTorque;					//Total net torque this instant
	Vector* previousNetTorque;			//net Torque previous instant
	Vector* netInstantaneousTorque;		//Total instantaneous torque this instant
	Vector* acceleration;				//current acceleration of point mass
	Vector* angularAcceleration;		//current angular acceleration
	Vector* velocity;					//current velocity of point mass
	Vector* angularVelocity;			//Current angular velocity
	FrameOfReference* frame;			//Position and orientation of point mass in global space
	unsigned char physicsOn;			//Boolean to turn physics off. 1 = on | 0 = off. Also a reference to the Human Torch. physicsOn!
} RigidBody;

///
//Allocates memory for a rigidBody
//
//Returns:
//	Pointer to a newly allocated RigidBody in memory
RigidBody* RigidBody_Allocate(void);

///
//Initializes a RigidBody
//
//PArameters:
//	body: THe rigid body to initialize
//	startingPosition: The position the rigidbody should initialize itself in world space
void RigidBody_Initialize(RigidBody* body, const Vector* startingPosition, const float mass);

///
//Frees resources allocated by a rigidbody
//
//Parameters:
//	body: The rigidbody to free
void RigidBody_Free(RigidBody* body);

///
//Uses a rigid bodies frame of reference to determine the cubes Width Depth and Height,
//Then uses them to calculate the inverse moment of inertia tensor.
//This function assumes that before scaling the cube has a space of -1 to 1 in all dimensions.
//
//Parameters:
//	body: The rigid body to calculate and set the inverse inertia tensor of
void RigidBody_SetInverseInertiaOfCuboid(RigidBody* body);

///
//Applies a force to a rigid body
//
//Parameter:
//	body: The rigid body to apply a force to
//	forceApplied: The force being applied
//	radius: The vector from the Center of Mass to the contact point on surface where force is applied
//		For purposes of preventing rotation make the radius 0.
void RigidBody_ApplyForce(RigidBody* body, const Vector* forceApplied, const Vector* radius);

///
//Applied an impuse to a rigid body
//
//Parameters:
//	body: The rigid body to apply an impulse to
//	impulseApplied: The impulse being applied
//	radius: The vector from the Center of Mass to the contact point on surface where force is applied
//		For purposes of preventing rotation make the radius 0.
void RigidBody_ApplyImpulse(RigidBody* body, const Vector* impulseApplied, const Vector* radius);

///
//Applies a torque to a rigidbody
//
//Parameters:
//	body: The body to apply a torque to
//	torqueApplied: The torque to apply
void RigidBody_ApplyTorque(RigidBody* body, const Vector* torqueApplied);

///
//Applies an instantaneous torque to a rigidbody
//
//Parameters:
//	body: The body to apply a torque to
//	torqueApplied: The instantaneous torque to apply
void RigidBody_ApplyInstantaneousTorque(RigidBody* body, const Vector* torqueApplied);

///
//Calculates the linear velocity due to the angular velocity of a point on/in a rigidbody.
//This is the instantaneous linear velocity of the point around the position of an object.
//
//Parameters:
//	dest: A pointer to a vector to store the linear velocity of the given point around the given body
//	body: A pointer to the rigidbody the given point is on/in
//	point: A pointer to a vector containing the point to calculate the local linear velocity of
void RigidBody_CalculateLocalLinearVelocity(Vector* dest, const RigidBody* body, const Vector* point);

///
//Calculates the maximum linear velocity due to the angular velocity of a set of points on/in a rigidbody in a specific direction.
//This is the max instantaneous linear velocity of the set of points around the position of the body which is in a certain direction.
//Dest will contain the zero vector in the case that none of the points are travelling in the given direction
//
//Parameters:
//	dest: A pointer to the vector to store the maximum linear velocity of the given point set around the given body in the given direction
//	body: A pointer to the rigidbody the given point set is on/in
//	points: A pointer to a dynamic array containing the given point set
//	direction: A pointer to a vector which contains the desired direction of maximum velocity
void RigidBody_CalculateMaxLocalLinearVelocity(Vector* dest, const RigidBody* body, const DynamicArray* points, const Vector* direction);

#endif