#include "TimeManager.h"

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <stdio.h>

#include "InputManager.h"


///
//Gets the internal time buffer from the time manager
//
//Returns:
//	Time buffer being managed by the time manager.
TimeBuffer TimeManager_GetTimeBuffer(void)
{
	return *timeBuffer;
}

///
//Initializes the Time Manager
void TimeManager_Initialize(void)
{
	timeBuffer = TimeManager_TimeBuffer_Allocate();
	TimeManager_TimeBuffer_Initialize(timeBuffer);
}

///
//Frees the time manager
void TimeManager_Free(void)
{
	TimeManager_TimeBuffer_Free(timeBuffer);
}

///
//Allocates a new time buffer
//
//Returns:
//	Pointer to a newly allocated uninitialized time buffer
TimeBuffer* TimeManager_TimeBuffer_Allocate(void)
{
	TimeBuffer* buffer = (TimeBuffer*)malloc(sizeof(TimeBuffer));
	return buffer;
}

///
//Initializes and starts a timebuffer
//
//Parameters:
//	buffer: timebuffer to initialize
void TimeManager_TimeBuffer_Initialize(TimeBuffer* buffer)
{
	buffer->ticksPerSecond = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	QueryPerformanceFrequency(buffer->ticksPerSecond);

	//buffer->startTime = glutGet(GLUT_ELAPSED_TIME);
	buffer->startTick = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	QueryPerformanceCounter(buffer->startTick);

	//buffer->elapsedTime = 0;
	buffer->elapsedTicks = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->elapsedTicks->QuadPart = 0L;

	buffer->deltaTicks = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->deltaTicks->QuadPart = 0L;
	
	buffer->elapsedTime = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->elapsedTime->QuadPart = 0L;

	buffer->previousTick = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->previousTick->QuadPart = buffer->startTick->QuadPart;
	
	//buffer->deltaTime = 0.0f;
	buffer->deltaTime = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->deltaTime->QuadPart = 0L;

	buffer->timeScale = 1.0f;
}

///
//Frees a time buffer
//
//Parameters:
//	buffer: Pointer to the buffer to free
void TimeManager_TimeBuffer_Free(TimeBuffer* buffer)
{
	free(buffer);
}

///
//Updates the time manager
void TimeManager_Update(void)
{

	TimeManager_UpdateBuffer(timeBuffer);

}

///
//Updates a time buffer
//
//Parameters:
//	buffer: Time buffer to update
void TimeManager_UpdateBuffer(TimeBuffer* buffer)
{
	LARGE_INTEGER currentTick;
	QueryPerformanceCounter(&currentTick);

	buffer->deltaTicks->QuadPart = (currentTick.QuadPart - buffer->previousTick->QuadPart);	
	buffer->deltaTime->QuadPart = (buffer->deltaTicks->QuadPart * 1000000.0f * buffer->timeScale) / buffer->ticksPerSecond->QuadPart;
	
	buffer->elapsedTicks->QuadPart += buffer->deltaTicks->QuadPart;
	buffer->elapsedTime->QuadPart = (buffer->elapsedTicks->QuadPart * 1000000.0) / buffer->ticksPerSecond->QuadPart;

	buffer->previousTick->QuadPart = currentTick.QuadPart;
}


///
//Sets the time manager's internal buffer's timeScale
//
//Parameters:
//	scale: The new time scale (closer to 0.0 is slower, higher than 1.0 is faster)
//		Don't put the scale below 0.. Time will go backwards.
void TimeManager_SetTimeScale(float scale)
{
	timeBuffer->timeScale = scale;
}

///
//Scales the time manager's internal buffer's timescale
//
//Parameters:
//	Scale: The scalar to multiply the current timescale by
void TimeManager_ScaleTimeScale(float scale)
{
	timeBuffer->timeScale *= scale;

}

///
//gets delta time in seconds as a single floating point
//
//Returns:
//	Number of seconds since last update
float TimeManager_GetDeltaSec(void)
{
	return timeBuffer->deltaTime->QuadPart / 1000000.0f;
}