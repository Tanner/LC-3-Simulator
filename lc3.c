/**
 * lc3.c
 * An LC-3 simulator backend.
 *
 * Author: Tanner Smith
 */

#include <stdbool.h>
#include "lc3.h"

/**
 * Inits the LC-3 machine.
 *
 * @param *state lc3machine
 */
void lc3_init(lc3machine* state) {
    state->pc = 0x3000;
    state->cc = LC3_ZERO;
    state->halted = false;
}

/**
 * Loads the program file into the LC-3 machine.
 *
 * @param *state lc3machine
 * @param *program Program to load
 */
void lc3_load(lc3machine* state, FILE* program) {
    lc3_init(state);

    bool instruction = false;
    short data = 0;
    short address = 0;

    short dataA = 0;
    short dataB = 0;

    while (true) {
        dataA = fgetc(program);
        dataB = fgetc(program);
        
        data = (dataA << 8) | dataB;

        if (dataA == EOF || dataB == EOF) {
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

/**
 * Execute one instruction.
 *
 * @param *state lc3machine
 */
void lc3_step_one(lc3machine* state) {
    if (!state->halted) {
        short instruction = lc3_fetch(state);

        lc3_execute(state, instruction);
    }
}

/**
 * Runs the machine x steps.
 *
 * @param *state lc3machine
 * @param num_steps Number of steps to run (x)
 */
void lc3_run(lc3machine* state, int num_steps) {
    if (num_steps >= 0) {
        for (int i = 0; i < num_steps && !state->halted; i++) {
            lc3_step_one(state);
        }
    } else if (num_steps == -1) {
        while (!state->halted) {
            lc3_step_one(state);
        }
    }
}

/**
 * Fetches the current instruction and increments the PC.
 *
 * @param *state lc3machine
 */
unsigned short lc3_fetch(lc3machine* state) {
    short instruction = state->mem[state->pc];

    state->pc++;

	return instruction;
}

/**
 * Execute the given instruction.
 *
 * @param *state lc3machine
 * @param instruction Instruction to run
 */
void lc3_execute(lc3machine* state, unsigned short instruction) {
    unsigned short opcode = lc3_get_opcode(instruction);

    if (opcode == 0x1) {
        // ADD
        unsigned short dest = BITS_11_9(instruction);
        unsigned short src1 = BITS_8_6(instruction);
        unsigned short bit5 = (instruction >> 5) & 0x1;

        unsigned short answer = 0;

        if (bit5 == 0) {
            // Non-IMM5
            unsigned short src2 = instruction & 0x7;

            answer = state->regs[src1] + state->regs[src2];
        } else {
            // IMM5
            short imm5 = SEXT5(BITS_4_0(instruction));

            answer = state->regs[src1] + imm5;
        }

        state->regs[dest] = answer;

        lc3_update_cc(state, answer);
    } else if (opcode == 0x5) {
        // AND
        unsigned short dest = BITS_11_9(instruction);
        unsigned short src1 = BITS_8_6(instruction);
        unsigned short bit5 = (instruction >> 5) & 0x1;

        unsigned short answer = 0;

        if (bit5 == 0) {
            // Non-IMM5
            unsigned short src2 = instruction & 0x7;

            answer = state->regs[src1] & state->regs[src2];
        } else {
            // IMM5
            short imm5 = SEXT5(BITS_4_0(instruction));

            answer = state->regs[src1] & imm5;
        }

        state->regs[dest] = answer;

        lc3_update_cc(state, answer);
    } else if (opcode == 0x0) {
        // BR
        unsigned char n = (instruction >> 11) & 0x1;
        unsigned char z = (instruction >> 10) & 0x1;
        unsigned char p = (instruction >> 9) & 0x1;

        short offset = SEXT9(BITS_8_0(instruction));

        unsigned char cc = state->cc;
        
        if ((p && cc == LC3_POSITIVE) || (z && cc == LC3_ZERO) || (n && cc == LC3_NEGATIVE)) {
            state->pc += offset;
        }
    } else if (opcode == 0xC) {
        // JMP or RET
        unsigned short reg = BITS_8_6(instruction);

        state->pc = state->regs[reg];
    } else if (opcode == 0x4) {
        // JSR or JSRR
        state->regs[7] = state->pc;

        unsigned short bit11 = (instruction >> 11) & 0x1;

        if (bit11) {
            // JSR
            short offset = SEXT11(instruction & 0x7FF);

            state->pc = state->pc + offset;
        } else {
            // JSRR
            
            unsigned short base = BITS_8_6(instruction);

            state->pc = state->regs[base];
        }
    } else if (opcode == 0x2) {
        // LD
        unsigned short dest = BITS_11_9(instruction);
        short offset = SEXT9(BITS_8_0(instruction));
         
        short result = state->mem[state->pc + offset];


        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0xA) {
        // LDI
        unsigned short dest = BITS_11_9(instruction);
        short offset = SEXT9(BITS_8_0(instruction));

        short result = state->mem[state->mem[state->pc + offset]];

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0x6) {
        // LDR
        unsigned short dest = BITS_11_9(instruction);
        unsigned short base = BITS_8_6(instruction);
        short offset = SEXT6(BITS_5_0(instruction));

        short result = state->mem[state->regs[base] + offset];

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0xE) {
        // LEA
        unsigned short dest = BITS_11_9(instruction);
        short offset = SEXT9(BITS_8_0(instruction));

        short result = state->pc + offset;

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0x9) {
        // NOT
        unsigned short dest = BITS_11_9(instruction);
        unsigned short src = BITS_8_6(instruction);

        short result = ~state->regs[src];

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0x3) {
        // ST
        unsigned short src = BITS_11_9(instruction);
        short offset = SEXT9(BITS_8_0(instruction));

        state->mem[state->pc + offset] = state->regs[src];
    } else if (opcode == 0xB) {
        // STI
        unsigned short src = BITS_11_9(instruction);
        short offset = SEXT9(BITS_8_0(instruction));

        state->mem[state->mem[state->pc + offset]] = state->regs[src];
    } else if (opcode == 0x7) {
        // STR
        unsigned short src = BITS_11_9(instruction);
        unsigned short base = BITS_8_6(instruction);
        short offset = SEXT6(BITS_5_0(instruction));

        state->mem[state->regs[base] + offset] = state->regs[src];
    } else if (opcode == 0xF) {
        // TRAP
        unsigned char vector = instruction & 0xEF;

        lc3_trap(state, vector);
    }
}

/**
 * Execute a specific TRAP.
 *
 * @param *state lc3machine
 * @param vector8 vector code
 */
void lc3_trap(lc3machine* state, unsigned char vector8) {
        if (vector8 == 0x20) {
            char buffer[256];
            char input = fgets((char *)&buffer, 256, stdin)[0];
            state->regs[0] = input;
        } else if (vector8 == 0x21) {
            // OUT
            printf("%c", state->regs[0]);
        } else if (vector8 == 0x22) {
            // PUTS
            int addr = state->regs[0];
            int value = 0;

            while ((value = state->mem[addr]) != 0x0000) {
                printf("%c", value);

                addr++;
            }
        } else if (vector8 == 0x23) {
            // IN
            printf("Input a character: ");

            char buffer[256];
            char input = fgets((char *)&buffer, 256, stdin)[0];
            state->regs[0] = input;
        } else if (vector8 == 0x24) {
            // PUTSP
            int addr = state->regs[0];
            int value = 0;

            while ((value = state->mem[addr]) != 0x0000) {
                printf("%c", value & 0x00FF);
                printf("%c", (value >> 8) & 0x00FF);

                addr++;
            }
        } else if (vector8 == 0x25) {
            // HALT
            state->halted = true;
            state->pc--;
        }
}

/**
 * Get the opcode from an instruction.
 *
 * @param instruction Instruction
 */
unsigned short lc3_get_opcode(unsigned short instruction) {
    return (instruction & 0xF000) >> 12;
}

/**
 * Update the CC from a result.
 *
 * @param *state lc3machine
 * @param answer Result to update the CC with
 */
void lc3_update_cc(lc3machine *state, short answer) {
    if (answer < 0) {
        state->cc = LC3_NEGATIVE;
    } else if (answer > 0) {
        state->cc = LC3_POSITIVE;
    } else {
        state->cc = LC3_ZERO;
    }
}

