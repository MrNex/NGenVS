#include "TimeScaleCommand.h"

#include <stdio.h>

#include "TimeManager.h"

///
//Initializes a command as a TimeScale Command.
//
//Parameters:
//	cmd: The command to initialize
void Command_TimeScale_Initialize(Command* cmd)
{
	cmd->Execute = Command_TimeScale_Execute;
	cmd->cue = "Scale\0";
}

///
//Execused a TimeScale command.
//Parses string arguments to the cue name (Scale) and a float
//Sending the float to the Time Manager as the new timescale
//
//Paramters:
//	cueArgs: Null terminated space delimited string containing the cue name followed by a float.
void Command_TimeScale_Execute(char* cueArgs)
{
	float scale = 0.0f;
	sscanf(cueArgs, "%*s %f", scale);
	TimeManager_SetTimeScale(scale);
}
