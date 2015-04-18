#include "Command.h"

#include <stdlib.h>

///
//Allocates memory for a new command
//
//Returns:
//	Pointer to a newly allocated uninitialized Command
Command* Command_Allocate(void)
{
	Command* cmd = (Command*)malloc(sizeof(Command));
	return cmd;
}

///
//Frees memory allocated by a command
//
//Parameters:
//	cmd: The command to free
void Command_Free(Command* cmd)
{
	free(cmd->cue);
	free(cmd);
}