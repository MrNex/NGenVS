//Unfortunately Windows Only for a little while...

#if defined(_WIN32) || defined(_WIN64)

#ifndef TIMEBUFFER_H
#define TIMEBUFFER_H

#include <windows.h>


typedef struct TimeBuffer
{
	LARGE_INTEGER* ticksPerSecond;
	LARGE_INTEGER* startTick;
	LARGE_INTEGER* elapsedTicks;
	LARGE_INTEGER* deltaTicks;

	LARGE_INTEGER* elapsedTime;		//In microSeconds
	LARGE_INTEGER* deltaTime;		//In microSeconds

	LARGE_INTEGER* previousTick;

	float timeScale;

} TimeBuffer;

//Internals
static TimeBuffer* timeBuffer;

//Functions

///
//Gets the internal time buffer from the time manager
//
//Returns:
//	Time buffer being managed by the time manager.
TimeBuffer TimeManager_GetTimeBuffer(void);

///
//Initializes the Time Manager
void TimeManager_Initialize(void);

///
//Frees the time manager
void TimeManager_Free(void);

///
//Allocates a new time buffer
//
//Returns:
//	Pointer to a newly allocated uninitialized time buffer
TimeBuffer* TimeManager_TimeBuffer_Allocate(void);

///
//Initializes and starts a timebuffer
//
//Parameters:
//	buffer: timebuffer to initialize
void TimeManager_TimeBuffer_Initialize(TimeBuffer* buffer);

///
//Frees a time buffer
//
//Parameters:
//	buffer: Pointer to the buffer to free
void TimeManager_TimeBuffer_Free(TimeBuffer* buffer);

///
//Updates the time manager
void TimeManager_Update(void);

///
//Updates a time buffer
//
//Parameters:
//	buffer: Time buffer to update
void TimeManager_UpdateBuffer(TimeBuffer* buffer);

///
//Sets the time manager's internal buffer's timeScale
//
//Parameters:
//	scale: The new time scale (closer to 0.0 is slower, higher than 1.0 is faster)
//	Don't put the scale below 0.. Time will go backwards.
void TimeManager_SetTimeScale(float scale);

///
//Scales the time manager's internal buffer's timescale
//
//Parameters:
//	Scale: The scalar to multiply the current timescale by
void TimeManager_ScaleTimeScale(float scale);



///
//gets delta time in seconds as a single floating point
//
//Returns:
//	Number of seconds since last update
float TimeManager_GetDeltaSec(void);



#endif	//If not defined

#endif	//If Windows