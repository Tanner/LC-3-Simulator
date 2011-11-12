/**
 * lc3sim.c
 * A front end to a LC-3 simulator.
 *
 * Author: Tanner Smith
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "lc3.h"

void split(char *input, char **split, char *delimiter, int count);
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
    assert(input);

    char *prev_input = malloc(sizeof(char) * 100);
    assert(prev_input);

    int max_args = 2;

    char **args = malloc(sizeof(char *) * max_args + 1);
    assert(args);

	while (1) {
        char *console_status;

        // Get input from user
		printf("%s", PROMPT);
        console_status = fgets(input, 100, stdin);

        if (console_status != NULL) {
            // Check to see if the user just pushed enter
            if (strcmp(input, "\n") == 0) {
                // If the user pushed enter, copy last command
                strncpy(input, prev_input, 100);
            } else {
                // If not, save the current command as last command
                strncpy(prev_input, input, 100);
            }

            // Input was successful so split input into command and args
            split(input, args, " \n", max_args + 1);

            // Grab the command and check it amongst all our commands
            char *command = args[0];

            if (command != NULL) {
                printf("Command: %s\n", command);
                if (strcmp(command, "step") == 0) {
                    // Step Command
                } else if (strcmp(command, "quit") == 0) {
                    // Quit command
                    break;
                } else if (strcmp(command, "continue") == 0) {
                    // Continue command
                } else if (strcmp(command, "registers") == 0) {
                    // Registers command
                    cmd_registers(&mach);
                } else if (strcmp(command, "dump") == 0) {
                    // Dump command
                } else if (strcmp(command, "setaddr") == 0) {
                    // Set Address command
                } else if (strcmp(command, "setreg") == 0) {
                    // Set Register command
                } else if (strcmp(command, "help") == 0) {
                    // Help command
                } else {
                    // Invalid command
                    printf("Undefined command: \"%s\". Try \"help\".\n", command);
                }
            }
        }

        mach.pc++;
	}

    free(input);
    free(prev_input);

    for (int i = 0; i < sizeof(args) / sizeof(char *); i++) {
        free(args[i]);
    }
    free(args);

	return 0;
}

void split(char *input, char **split, char *delimiter, int count) {
    char *temp = strtok(input, delimiter);

    for (int i = 0; i < count && temp != NULL; i++) {
        split[i] = malloc(strlen(temp) + 1);
        assert(split[i]);

        strcpy(split[i], temp);

        temp = strtok(NULL, delimiter);
    }
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
        printf("0x%.4X: 0x%.16X\n", start, mach->mem[start]);
    } else if (start >= end) {
        for (int i = start; i <= end; i++) {
            printf("0x%.4X: 0x%.16X\n", start, mach->mem[start]);
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

    printf("Set memory location 0x%.4X to 0x%.16X\n", address, value);
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
