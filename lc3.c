/**
 * lc3.c
 * An LC-3 simulator backend.
 *
 * Author: Tanner Smith
 */

#include "lc3.h"

void lc3_init(lc3machine* state) {
    state->pc = 0x3000;
    state->cc = LC3_ZERO;
}

void lc3_load(lc3machine* state, FILE* program) {
    lc3_init(state);
	/* FIXME: Add in the code to initialize the memory according to the
	          contents of the program as well as the initial state of the
			  machine's registers. */
	// Reread the section on the .obj file format.
    short data = 0;

    while (data != EOF) {
        data = (fgetc(program) << 8) | fgetc(program);

        if (data == EOF) {
            break;
        }

        printf("%.16X\n", data);
    }
}

void lc3_step_one(lc3machine* state) {
	// If the machine is not halted
	// Fetch an instruction
	// And call lc3_execute 
}

void lc3_run(lc3machine* state, int num_steps) {
}

unsigned short lc3_fetch(lc3machine* state) {
	/* Think back to when we first started assembly to code this */
	/* What happens during the fetch stage? */
	return 0;
}

void lc3_execute(lc3machine* state, unsigned short instruction) {
}

void lc3_trap(lc3machine* state, unsigned char vector8) {
}
