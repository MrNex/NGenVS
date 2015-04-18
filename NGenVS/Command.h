#ifndef COMMAND_H
#define COMMAND_H

//Made up of two parts
//	The action of the command, or what operations are performed when it's triggered
//	The Cue, or thing that triggers this command to be ran
typedef struct Command
{
	char* cue;
	void(*Execute)(char* cueArgs);	//Executes the action after parsing cue and arguments

} Command;

union Action
{
	void(*vAction_v)(void);		//Action which returns and takes nothing
	void(*vAction_f)(float);	//Returns nothing takes a float
};

///
//Allocates memory for a new command
//
//Returns:
//	Pointer to a newly allocated uninitialized Command
Command* Command_Allocate(void);

///
//Frees memory allocated by a command
//
//Parameters:
//	cmd: The command to free
void Command_Free(Command* cmd);

#endif