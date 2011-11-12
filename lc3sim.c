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

void cmd_registers(lc3machine *mach);
void cmd_dump(lc3machine *mach, int start, int end);
void cmd_setaddr(lc3machine *mach, int address, short value);
void cmd_setreg(lc3machine *mach, int reg, short value);

#define PROMPT "(lc-3) "

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
    char *input = malloc(sizeof(char) * 100);

	while (1) {
        char *console_status;

		printf("%s", PROMPT);
        console_status = fgets(input, 100, stdin);

        if (console_status != NULL || *console_status != EOF) {
            char *command = strtok(input, " ");

            char *command_args;

            printf("Command: %s\n", command);
            if (command != NULL) {
                do {
                    command_args = strtok(NULL, " ");
                    printf("Arg: %s\n", command_args);
                } while (command_args != NULL);
            }
        }

        mach.pc++;
	}

    free(input);

	return 0;
}

/* cmd_step and cmd_continue 's functionality are provided in lc3_run
Therefore to execute the step and coninute commands you can just call lc3_run with the correct parameters*/

/* cmd_registers
Should print out all of the registers and the PC and CC in both hex and signed decimal.
*/
void cmd_registers(lc3machine *mach) {
    printf("PC: 0x%.6X (%d)\n", mach->pc, mach->pc);
    printf("CC: 0x%.6X (%d)\n", mach->cc, mach->cc);

    for (int i = 0; i < sizeof(mach->regs) / sizeof(short); i++) {
        printf("R%d: 0x%.6X (%d)\n", i, mach->regs[i], mach->regs[i]);
    }
}

/* cmd_dump
Should print out the contents of memory from start to end
If end is -1 then just print out memory[start]*/
void cmd_dump(lc3machine *mach, int start, int end) {
    if (end == -1) {
        printf("%.4X: %.16X\n", start, mach->mem[start]);
    } else if (start >= end) {
        for (int i = start; i <= end; i++) {
            printf("%.4X: %.16X\n", start, mach->mem[start]);
        }
    }
}

/* cmd_setaddr
 Should set a memory address to some value
*/
void cmd_setaddr(lc3machine *mach, int address, short value) {
    // Check the bounds of address first
    if (address < 0 || address > sizeof(mach->mem) / sizeof(short)) {
        return;
    }

    mach->mem[address] = value;

    printf("Set memory location %.4X to %.16X\n", address, value);
}

/* cmd_setreg
  Should set a register to some value passed in
*/
void cmd_setreg(lc3machine *mach, int reg, short value) {
    // Check the bounds of the register first
    if (reg < 0 || reg > sizeof(mach->regs) / sizeof(short)) {
        return;
    }

    mach->regs[reg] = value;

    printf("Set R%d to %.16X\n", reg, value);
}
