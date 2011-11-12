/**
 * lc3sim.c
 * A front end to a LC-3 simulator.
 *
 * Author: Tanner Smith
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lc3.h"

void cmd_registers(void);
void cmd_dump(int start, int end);
void cmd_setaddr(int address, short value);
void cmd_setreg(int reg, short value);

#define PROMPT "(lc-3)"

int main(int argc, char **argv) {
	FILE *prog;
	lc3machine mach;

	/* We expect only one argument for the program... */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file.obj\n", argv[0]);
		return 1;
	}
	/* We want to open the file and make sure that it exists. */
	prog = fopen(argv[1], "rb");
	if (!prog) {
		fprintf(stderr, "Could not find file %s\n", argv[1]);
		return 2;
	}

	/* Make a call to lc3_init to initialize your lc3machine */
	/* Make a call to lc3_load to load the program */

	printf("LC-3 Simulator and Debugger\n");
	printf("Written by Tanner Smith\n");

	/* Run this loop until we are told to stop debugging. */
	while (1) {
		/* FIXME: Add some necessary variables here */
		printf("%s", PROMPT);
		/* FIXME: Read in user commands and execute them! */
	}

	return 0;
}

/* cmd_step and cmd_continue 's functionality are provided in lc3_run
Therefore to execute the step and coninute commands you can just call lc3_run with the correct parameters*/

/* cmd_registers
Should print out all of the registers and the PC and CC in both hex and signed decimal.
*/
void cmd_registers(void) {
}

/* cmd_dump
Should print out the contents of memory from start to end
If end is -1 then just print out memory[start]*/
void cmd_dump(int start, int end) {
}

/* cmd_setaddr
 Should set a memory address to some value
*/
void cmd_setaddr(int address, short value) {
}

/* cmd_setreg
  Should set a register to some value passed in
*/
void cmd_setreg(int reg, short value) {
}
