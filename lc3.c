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
    state->halted = false;
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
    if (!state->halted) {
        short instruction = lc3_fetch(state);

        lc3_execute(state, instruction);
    }
}

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

unsigned short lc3_fetch(lc3machine* state) {
    short instruction = state->mem[state->pc];

    state->pc++;

	return instruction;
}

void lc3_execute(lc3machine* state, unsigned short instruction) {
    unsigned short opcode = lc3_get_opcode(instruction);

    if (opcode == 0x1) {
        // ADD
        unsigned short dest = lc3_get_11_to_9(instruction);
        unsigned short src1 = lc3_get_8_to_6(instruction);
        unsigned short bit5 = (instruction >> 5) & 0x1;

        unsigned short answer = 0;

        if (bit5 == 0) {
            // Non-IMM5
            unsigned short src2 = instruction & 0x7;

            answer = state->regs[src1] + state->regs[src2];
        } else {
            // IMM5
            unsigned short imm5 = instruction & 0x1F;

            answer = state->regs[src1] + SEXT5(imm5);
        }

        state->regs[dest] = answer;

        lc3_update_cc(state, answer);
    } else if (opcode == 0x5) {
        // AND
        unsigned short dest = lc3_get_11_to_9(instruction);
        unsigned short src1 = lc3_get_8_to_6(instruction);
        unsigned short bit5 = (instruction >> 5) & 0x1;

        unsigned short answer = 0;

        if (bit5 == 0) {
            // Non-IMM5
            unsigned short src2 = instruction & 0x7;

            answer = state->regs[src1] & state->regs[src2];
        } else {
            // IMM5
            unsigned short imm5 = instruction & 0x1F;

            answer = state->regs[src1] & SEXT5(imm5);
        }

        state->regs[dest] = answer;

        lc3_update_cc(state, answer);
    } else if (opcode == 0x0) {
        // BR
        unsigned char n = (instruction >> 11) & 0x1;
        unsigned char z = (instruction >> 10) & 0x1;
        unsigned char p = (instruction >> 9) & 0x1;

        unsigned short offset = lc3_get_8_to_0(instruction);

        unsigned char cc = state->cc;
        
        if ((p && cc == LC3_POSITIVE) || (z && cc == LC3_ZERO) || (n && cc == LC3_NEGATIVE)) {
            state->pc += SEXT9(offset);
        }
    } else if (opcode == 0xC) {
        // JMP or RET
        unsigned short reg = lc3_get_8_to_6(instruction);
        state->pc = state->regs[reg];
    } else if (opcode == 0x4) {
        // JSR or JSRR
        state->regs[6] = state->pc;

        unsigned short bit11 = instruction & 0x400;

        if (bit11) {
            // JSR
            unsigned short offset = instruction & 0x7FF;

            state->pc += SEXT11(offset);
        } else {
            // JSRR
            
            unsigned short base = lc3_get_8_to_6(instruction);

            state->pc = state->regs[base];
        }
    } else if (opcode == 0x2) {
        // LD
        unsigned short dest = lc3_get_11_to_9(instruction);
        unsigned short offset = lc3_get_8_to_0(instruction);

        short result = state->mem[state->pc + SEXT9(offset)];

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0xA) {
        // LDI
        unsigned short dest = lc3_get_11_to_9(instruction);
        unsigned short offset = lc3_get_8_to_0(instruction);

        short result = state->mem[state->mem[state->pc + SEXT9(offset)]];

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0x6) {
        // LDR
        unsigned short dest = lc3_get_11_to_9(instruction);
        unsigned short base = lc3_get_8_to_6(instruction);
        unsigned short offset = lc3_get_5_to_0(instruction);

        short result = state->mem[base + SEXT6(offset)];

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0xE) {
        // LEA
        unsigned short dest = lc3_get_11_to_9(instruction);
        unsigned short offset = lc3_get_8_to_0(instruction);

        short result = state->pc + SEXT9(offset);

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0x9) {
        // NOT
        unsigned short dest = lc3_get_11_to_9(instruction);
        unsigned short src = lc3_get_8_to_6(instruction);

        short result = ~state->regs[src];

        state->regs[dest] = result;

        lc3_update_cc(state, result);
    } else if (opcode == 0x3) {
        // ST
        unsigned short src = lc3_get_11_to_9(instruction);
        unsigned short offset = lc3_get_8_to_0(instruction);

        state->mem[state->pc + SEXT9(offset)] = state->regs[src];
    } else if (opcode == 0xB) {
        // STI
        unsigned short src = lc3_get_11_to_9(instruction);
        unsigned short offset = lc3_get_8_to_0(instruction);

        state->mem[state->mem[state->pc + SEXT9(offset)]] = state->regs[src];
    } else if (opcode == 0x7) {
        // STR
        unsigned short src = lc3_get_11_to_9(instruction);
        unsigned short base = lc3_get_8_to_6(instruction);
        unsigned short offset = lc3_get_5_to_0(instruction);

        state->mem[state->regs[base] + SEXT6(offset)] = state->mem[src];
    } else if (opcode == 0xF) {
        // TRAP
        unsigned short vector = (instruction >> 8) & 0xF;

        if (vector == 0x25) {
            // HALT
            state->halted = true;
            state->pc--;
        }
    }
}

void lc3_trap(lc3machine* state, unsigned char vector8) {
}

unsigned short lc3_get_opcode(unsigned short instruction) {
    return (instruction & 0xF000) >> 12;
}

unsigned short lc3_get_11_to_9(unsigned short instruction) {
    return (instruction >> 9) & 0x7;
}

unsigned short lc3_get_8_to_6(unsigned short instruction) {
    return (instruction >> 6) & 0x7;
}

unsigned short lc3_get_8_to_0(unsigned short instruction) {
    return instruction & 0x1FF;
}

unsigned short lc3_get_5_to_0(unsigned short instruction) {
    return instruction & 0x1F;
}

void lc3_update_cc(lc3machine *state, unsigned short answer) {
    if (answer < 0) {
        state->cc = LC3_NEGATIVE;
    } else if (answer > 0) {
        state->cc = LC3_POSITIVE;
    } else {
        state->cc = LC3_ZERO;
    }
}

