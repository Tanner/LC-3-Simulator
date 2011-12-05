/**
 * lc3.c
 * An LC-3 simulator backend.
 *
 * Author: Tanner Smith
 */

#include <stdbool.h>
#include "lc3.h"

void lc3_init(lc3machine* state) {
    state->pc = 0x3000;
    state->cc = LC3_ZERO;
    state->halt = false;
}

void lc3_load(lc3machine* state, FILE* program) {
    lc3_init(state);

    bool instruction = false;
    short data = 0;
    short address = 0;

    while (data != EOF) {
        data = (fgetc(program) << 8) | fgetc(program);

        if (data == EOF) {
            break;
        }

        if (!instruction) {
            address = data;
            state->pc = address;

            instruction = true;
        } else {
            state->mem[address] = data;
            address++;
        }
    }
}

void lc3_step_one(lc3machine* state) {
	// If the machine is not halted
	// Fetch an instruction
	// And call lc3_execute 
    if (state->halt) {
        short instruction = lc3_fetch(state);
        state->pc++;

        lc3_execute(state, instruction);
    }
}

void lc3_run(lc3machine* state, int num_steps) {
    if (num_steps >= 0) {
        for (int i = 0; i < num_steps && !state->halt; i++) {
            lc3_step_one(state);
        }
    } else if (num_steps == -1) {
        while (!state->halt) {
            lc3_step_one(state);
        }
    }
}

unsigned short lc3_fetch(lc3machine* state) {
    short instruction = state->mem[state->pc];

    state->pc++;

	return instruction;
}

void lc3_execute(lc3machine* state, unsigned short instruction) {
}

void lc3_trap(lc3machine* state, unsigned char vector8) {
}
