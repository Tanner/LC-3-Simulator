#include "lc3.h"

void lc3_init(lc3machine* state) 
{
	// Initialize the lc3 state according to the assignment.
}

void lc3_load(lc3machine* state, FILE* program)
{
	/* FIXME: Add in the code to initialize the memory according to the
	          contents of the program as well as the initial state of the
			  machine's registers. */
	// Reread the section on the .obj file format.
}

void lc3_step_one(lc3machine* state)
{
	// If the machine is not halted
	// Fetch an instruction
	// And call lc3_execute 
}

void lc3_run(lc3machine* state, int num_steps)
{
}

unsigned short lc3_fetch(lc3machine* state)
{
	/* Think back to when we first started assembly to code this */
	/* What happens during the fetch stage? */
	return 0;
}

void lc3_execute(lc3machine* state, unsigned short instruction)
{
}

void lc3_trap(lc3machine* state, unsigned char vector8)
{
}
