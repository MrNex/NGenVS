#ifndef COMMAND_TIMESCALE_H
#define COMMAND_TIMESCALE_H

#include "Command.h"

///
//Initializes a command as a TimeScale Command.
//
//Parameters:
//	cmd: The command to initialize
void Command_TimeScale_Initialize(Command* cmd);

///
//Execused a TimeScale command.
//Parses string arguments to the cue name (Scale) and a float
//Sending the float to the Time Manager as the new timescale
//
//Paramters:
//	cueArgs: Null terminated space delimited string containing the cue name followed by a float.
void Command_TimeScale_Execute(char* cueArgs);


#endif