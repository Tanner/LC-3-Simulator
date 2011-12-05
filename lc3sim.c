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
int htoi(char *str);
int hex_letter_to_int(char letter);
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

    lc3_init(&mach);
    lc3_load(&mach, prog);

    fclose(prog);

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
                if (strcmp(command, "step") == 0) {
                    // Step Command
                    if (args[1] == NULL) {
                        lc3_run(&mach, 1);
                    } else {
                        int n = *args[1] - '0';
                        lc3_run(&mach, n);
                    }
                } else if (strcmp(command, "quit") == 0) {
                    // Quit command
                    break;
                } else if (strcmp(command, "continue") == 0) {
                    // Continue command
                    lc3_run(&mach, -1);
                } else if (strcmp(command, "registers") == 0) {
                    // Registers command
                    cmd_registers(&mach);
                } else if (strcmp(command, "dump") == 0) {
                    // Dump command
                    if (args[1] == NULL) {
                        printf("Usage: dump start [end]\n");
                    } else {
                        int start = htoi(args[1]);

                        if (args[2] == NULL && start != -1) {
                            cmd_dump(&mach, start, -1);
                        } else if (args[2] != NULL) {
                            int end = htoi(args[2]);
                            
                            if (end != -1) {
                                cmd_dump(&mach, start, end);
                            } else {
                                printf("Invalid memory address: %s\n", args[2]);
                            }
                        } else {
                            printf("Invalid memory address: %s\n", args[1]);
                        }
                    }
                } else if (strcmp(command, "setaddr") == 0) {
                    // Set Address command
                    if (args[1] == NULL && args[2] == NULL) {
                        printf("Usage: setaddr addr value\n");
                    } else {
                        int address = htoi(args[1]);
                        short value = atoi(args[2]);

                        if (address != -1) {
                            cmd_setaddr(&mach, address, value);
                        } else {
                            printf("Invalid memory address: %s\n", args[1]);
                        }
                    }
                } else if (strcmp(command, "setreg") == 0) {
                    // Set Register command
                    if (args[1] == NULL || args[2] == NULL) {
                        printf("Usage: setreg Rn value\n");
                    } else {
                        if (args[1][0] == 'R' || args[1][0] == 'r') {
                            int reg = args[1][1] - '0';
                            int value = atoi(args[2]);

                            if (reg >= 1 && reg <= 8) {
                                cmd_setreg(&mach, reg, value);
                            } else {
                                printf("Invalid register: %d\n", reg);
                            }
                        } else {
                            printf("Usage: setreg Rn value\n");
                        }
                    }
                } else if (strcmp(command, "help") == 0) {
                    // Help command
                    printf("List of commands:\n\n");
                    printf("step [n]\t\tn is the number of steps; -1 for infinite\n");
                    printf("quit\t\t\tQuits the simulator\n");
                    printf("continue\t\tRuns the simulator until halt\n");
                    printf("registers\t\tPrints all registers, the CC, and PC to the screen\n");
                    printf("dump start [end]\tDumps all memory from start to end\n");
                    printf("setaddr addr val\tSets memory address (hex) to val (dec)\n");
                    printf("setreg Rn val\t\tSets register n (1 to 8) to val (dec)\n");
                    printf("\n[ ] represent an argument that is not required\n\n");
                } else {
                    // Invalid command
                    printf("Undefined command: \"%s\". Try \"help\".\n", command);
                }
            }
        }
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

    for (int i = 0; i < count; i++) {
        if (split[i] != NULL) {
            free(split[i]);

            split[i] = NULL;
        }

        if (temp != NULL) {
            split[i] = malloc(strlen(temp) + 1);
            assert(split[i]);

            strcpy(split[i], temp);
        }

        temp = strtok(NULL, delimiter);
    }
}

int htoi(char *str) {
    int i = 0;

    if (str[i] == '0') {
        i++;
        if (str[i] == 'x' || str[i] == 'X') {
            i++;
        }
    }

    if (i == 2) {
        // Good so far got the 0x
        int answer = 0;

        while (i < strlen(str)) {
            answer *= 16;

            if (str[i] >= '0' && str[i] <= '9') {
                answer += str[i] - '0';
            } else {
                int number = hex_letter_to_int(str[i]);
                answer += number;

                if (number == 0) {
                    return -1;
                }
            }

            i++;
        }

        return answer;
    } else {
        return -1;
    }
}

int hex_letter_to_int(char letter) {
    if (letter & 0x20) {
        // Letter is lowercase
        if (letter >= 97 && letter <= 102) {
            // a to f
            return 10 + (letter - 97);
        }
    } else {
        // Letter is uppercase
        if (letter >= 65 && letter <= 70) {
            // A to F
            return 10 + (letter - 65);
        }
    }

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
        printf("R%d: 0x%.6X (%d)\n", i + 1, mach->regs[i], mach->regs[i]);
    }
}

/* cmd_dump
Should print out the contents of memory from start to end
If end is -1 then just print out memory[start]*/
void cmd_dump(lc3machine *mach, int start, int end) {
    if (end == -1) {
        printf("0x%.4X: 0x%.16X\n", start, mach->mem[start]);
    } else if (start <= end) {
        for (int i = start; i <= end; i++) {
            printf("0x%.4X: 0x%.16X\n", i, mach->mem[i]);
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

    printf("Set memory location 0x%.4X to 0x%.16X (%d)\n", address, value, value);
}

/* cmd_setreg
  Should set a register to some value passed in
*/
void cmd_setreg(lc3machine *mach, int reg, short value) {
    // Check the bounds of the register first
    if (reg < 1 || reg > sizeof(mach->regs) / sizeof(short)) {
        return;
    }

    mach->regs[reg - 1] = value;

    printf("Set R%d to 0x%.16X (%d)\n", reg, value, value);
}
