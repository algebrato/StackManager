#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Tokenize helper function
int tokenize(const char* line, char tokens[][16], int maxTokens) {
    int count = 0;
    const char* start = line;
    while (*start && count < maxTokens) {
        while (*start == ' ') start++; // Skip spaces
        if (*start == '\0') break;

        const char* end = start;
        while (*end && *end != ' ') end++; // Find end of token

        int length = end - start;
        if (length > 15) length = 15; // Limit token size
        strncpy(tokens[count], start, length);
        tokens[count][length] = '\0';
        count++;

        start = end;
    }
    return count;
}

// Opcode map and operand count
typedef struct {
    const char* name;
    uint8_t opcode;
    int operands;
} Instruction;

Instruction instructions[] = {
    {"LOAD", 0x01, 2}, {"ADD", 0x02, 2}, {"SUB", 0x03, 2},
    {"STORE", 0x04, 2}, {"NOP", 0x00, 0}, {"AND", 0x06, 2},
    {"OR", 0x07, 2}, {"XOR", 0x08, 2}, {"READ", 0x09, 1},
    {"WRITE", 0x0A, 1}, {"JMP", 0x0B, 1}, {"CALL", 0x0C, 1},
    {"RET", 0x0D, 0}, {"BEQ", 0x0E, 1}, {"BNE", 0x0F, 1},
    {"INT", 0x10, 0}
};
#define INSTRUCTION_COUNT (sizeof(instructions) / sizeof(Instruction))

// Find instruction by name
Instruction* findInstruction(const char* name) {
    for (int i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strcmp(instructions[i].name, name) == 0) {
            return &instructions[i];
        }
    }
    return NULL;
}

// Assemble function: Converts instructions into binary format
int assembleBinary(const char* assembly[], int assemblySize, uint8_t* machineCode, int maxCodeSize) {
    int codeIndex = 0;
    char tokens[10][16]; // Max 10 tokens per line, each token up to 15 characters

    for (int i = 0; i < assemblySize; i++) {
        int tokenCount = tokenize(assembly[i], tokens, 10);
        if (tokenCount == 0) continue;

        Instruction* instr = findInstruction(tokens[0]);
        if (!instr) {
            printf("Error: Unknown instruction: %s\n", tokens[0]);
            continue;
        }

        if (codeIndex >= maxCodeSize) break;
        machineCode[codeIndex++] = instr->opcode;

        if (tokenCount - 1 != instr->operands) {
            printf("Error: Incorrect number of operands for %s\n", tokens[0]);
            continue;
        }

        for (int j = 1; j < tokenCount; j++) {
            if (codeIndex >= maxCodeSize) break;
            machineCode[codeIndex++] = (uint8_t*)atoi(tokens[j]);
        }
    }
    return codeIndex;
}

// Memory structure
typedef struct {
    uint8_t mem[256];
} Memory;

void memoryWrite(Memory* memory, uint8_t address, uint8_t value) {
    if (address < 256) {
        memory->mem[address] = value;
    } else {
        printf("Error: Invalid memory address %d\n", address);
    }
}

uint8_t memoryRead(Memory* memory, uint8_t address) {
    if (address < 256) {
        return memory->mem[address];
    } else {
        printf("Error: Invalid memory address %d\n", address);
        return 0;
    }
}

void displayMemory(Memory* memory) {
    for (int i = 0; i < 256; i++) {
        if (memory->mem[i] != 0) {
            printf("Memory[%d]: %d\n", i, memory->mem[i]);
        }
    }
}

// CPU structure
typedef struct {
    uint8_t pc;       // Program counter
    uint8_t ir;       // Instruction register
    uint8_t reg[4];   // Four general-purpose registers
    Memory memory;
    uint8_t callStack[256];
    int stackPointer;
} CPU;

void execute(CPU* cpu, const uint8_t* machineCode, int codeSize) {
    clock_t start = clock();

    while (cpu->pc < codeSize) {
        cpu->ir = machineCode[cpu->pc++];
        uint8_t opcode = cpu->ir;

        switch (opcode) {
            case 0x01: { // LOAD
                uint8_t regNum = machineCode[cpu->pc++];
                uint8_t value = machineCode[cpu->pc++];
                cpu->reg[regNum] = value;
                break;
            }
            case 0x04: { // STORE
                uint8_t regNum = machineCode[cpu->pc++];
                uint8_t address = machineCode[cpu->pc++];
                memoryWrite(&cpu->memory, address, cpu->reg[regNum]);
                break;
            }
            case 0x02: { // ADD
                uint8_t regNum1 = machineCode[cpu->pc++];
                uint8_t regNum2 = machineCode[cpu->pc++];
                cpu->reg[regNum1] += cpu->reg[regNum2];
                break;
            }
            case 0x03: { // SUB
                uint8_t regNum1 = machineCode[cpu->pc++];
                uint8_t regNum2 = machineCode[cpu->pc++];
                cpu->reg[regNum1] -= cpu->reg[regNum2];
                break;
            }
            case 0x0A: { // WRITE
                uint8_t regNum = machineCode[cpu->pc++];
                printf("Output: %d\n", cpu->reg[regNum]);
                break;
            }
            case 0x09: { // READ
                uint8_t regNum = machineCode[cpu->pc++];
                int input;
                printf("Input: ");
                scanf("%d", &input);
                cpu->reg[regNum] = (uint8_t)input;
                break;
            }
            case 0x0B: { // JMP
                uint8_t address = machineCode[cpu->pc++];
                cpu->pc = address;
                break;
            }
            case 0x0C: { // CALL
                uint8_t address = machineCode[cpu->pc++];
                cpu->callStack[cpu->stackPointer++] = cpu->pc;
                cpu->pc = address;
                break;
            }
            case 0x0D: { // RET
                if (cpu->stackPointer > 0) {
                    cpu->pc = cpu->callStack[--cpu->stackPointer];
                } else {
                    printf("Error: Call stack underflow!\n");
                }
                break;
            }
            case 0x0E: { // BEQ
                uint8_t regNum = machineCode[cpu->pc++];
                uint8_t address = machineCode[cpu->pc++];
                if (cpu->reg[regNum] == 0) {
                    cpu->pc = address;
                }
                break;
            }
            case 0x0F: { // BNE
                uint8_t regNum = machineCode[cpu->pc++];
                uint8_t address = machineCode[cpu->pc++];
                if (cpu->reg[regNum] != 0) {
                    cpu->pc = address;
                }
                break;
            }
            case 0x10: { // INT
                printf("Interrupt triggered!\n");
                break;
            }
            case 0x00: { // NOP
                break;
            }
            default:
                printf("Error: Unknown opcode %d\n", opcode);
        }
    }

    clock_t stop = clock();
    printf("Execution time: %ld microseconds\n", (stop - start) * 1000000 / CLOCKS_PER_SEC);
}

void displayState(CPU* cpu) {
    printf("Registers:\n");
    for (int i = 0; i < 4; i++) {
        printf("R%d: %d\n", i, cpu->reg[i]);
    }
    printf("Memory State:\n");
    displayMemory(&cpu->memory);
}

int main() {
    // Example assembly program
    const char* assembly[] = {
        "LOAD 0 15",   // Load 15 into R0
        "LOAD 1 8",    // Load 8 into R1
        "ADD 0 1",     // Add R1 to R0
    };
    int assemblySize = sizeof(assembly) / sizeof(assembly[0]);

    // Assemble the program into binary
    uint8_t machineCode[256];
    int codeSize = assembleBinary(assembly, assemblySize, machineCode, 256);

    printf("Machine code (Binary):\n");
    for (int i = 0; i < codeSize; i++) {
        printf("%02X ", machineCode[i]);
    }
    printf("\n");

    // Run the program
    CPU cpu = {0};
    execute(&cpu, machineCode, codeSize);

    // Display the final state
    displayState(&cpu);

    return 0;
}
