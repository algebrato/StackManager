#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "StackManager.h"

// Define the register stack and stack pointer
static uint32_t register_stack[REGISTER_STACK_SIZE];
static int register_stack_pointer = -1;

// Define the program stack and stack pointer
static uint32_t program_stack[PROGRAM_STACK_SIZE];
static int program_stack_pointer = -1;

// Function to initialize the register stack with random values
void initialize_register_stack() {
    srand(time(NULL));
    for (int i = 0; i < REGISTER_STACK_SIZE; i++) {
        register_stack[i] = rand();
    }
    register_stack_pointer = REGISTER_STACK_SIZE - 1;
}

// // Function to initialize the program stack with random values
// void initialize_program_stack() {
//     srand(time(NULL));
//     for (int i = 0; i < PROGRAM_STACK_SIZE; i++) {
//         program_stack[i] = rand();
//     }
//     program_stack_pointer = PROGRAM_STACK_SIZE - 1;
// }

// Function to print the register stack
void print_register_stack() {
    for (int i = 0; i < REGISTER_STACK_SIZE; i++) {
        printf("register_stack[%d] = %u\n", i, register_stack[i]);
    }
    printf("Register Stack Pointer: %d\n", register_stack_pointer);
}

// Function to print the program stack
void print_program_stack() {
    for (int i = 0; i < PROGRAM_STACK_SIZE; i++) {
        printf("program_stack[%d] = %u\n", i, program_stack[i]);
    }
    printf("Program Stack Pointer: %d\n", program_stack_pointer);
}

// Function to save register stack to program stack
void save_registers_to_stack() {
    if (program_stack_pointer + REGISTER_STACK_SIZE >= PROGRAM_STACK_SIZE) {
        printf("Error: Program stack overflow\n");
        return;
    }
    for (int i = 0; i < REGISTER_STACK_SIZE; i++) {
        program_stack[++program_stack_pointer] = register_stack[i];
    }
}

void restore_registers_from_stack() {
    if (program_stack_pointer - REGISTER_STACK_SIZE < -1) {
        printf("Error: Program stack underflow\n");
        return;
    }
    for (int i = REGISTER_STACK_SIZE - 1; i >= 0; i--) {
        register_stack[i] = program_stack[program_stack_pointer--];
    }
}

// Function ciao that takes two integer parameters
void ciao(int a, int b) {
    printf("ciao function called with parameters: %d, %d\n", a, b);
}

// Function add that takes two double parameters and returns their sum
double add(double x, double y) {
    return x + y;
}

// Function multiply that takes two integer parameters and returns their product
int multiply(int x, int y) {
    return x * y;
}

// Function to_upper that takes a char parameter and returns its uppercase version
char to_upper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}

// Define the function map
FunctionInfo function_map[] = {
    {"ciao", (void (*)())ciao, {"int", "int"}, {"a", "b"}, 0},
    {"add", (void (*)())add, {"double", "double"}, {"x", "y"}, sizeof(double)},
    {"multiply", (void (*)())multiply, {"int", "int"}, {"x", "y"}, sizeof(int)},
    {"to_upper", (void (*)())to_upper, {"char"}, {"c"}, sizeof(char)}
};

void invoke_function(const char *name) {
    for (int i = 0; i < sizeof(function_map) / sizeof(FunctionInfo); i++) {
        if (strcmp(function_map[i].name, name) == 0) {
            save_registers_to_stack(); // Save registers before invoking the function

            void *params[2] = {0};
            for (int j = 0; j < 2 && function_map[i].param_types[j] != NULL; j++) {
                if (strcmp(function_map[i].param_types[j], "int") == 0) {
                    params[j] = malloc(sizeof(int));
                    printf("Enter parameter %s (int): ", function_map[i].param_names[j]);
                    scanf("%d", (int *)params[j]);
                } else if (strcmp(function_map[i].param_types[j], "double") == 0) {
                    params[j] = malloc(sizeof(double));
                    printf("Enter parameter %s (double): ", function_map[i].param_names[j]);
                    scanf("%lf", (double *)params[j]);
                } else if (strcmp(function_map[i].param_types[j], "char") == 0) {
                    params[j] = malloc(sizeof(char));
                    printf("Enter parameter %s (char): ", function_map[i].param_names[j]);
                    scanf(" %c", (char *)params[j]);
                }
            }

            void *result = NULL;
            if (function_map[i].return_size == 0) {
                ((void (*)(int, int))function_map[i].func_ptr)(*(int *)params[0], *(int *)params[1]);
            } else if (function_map[i].return_size == sizeof(double)) {
                result = malloc(sizeof(double));
                *(double *)result = ((double (*)(double, double))function_map[i].func_ptr)(*(double *)params[0], *(double *)params[1]);
                printf("Result: %f\n", *(double *)result);
            } else if (function_map[i].return_size == sizeof(int)) {
                result = malloc(sizeof(int));
                *(int *)result = ((int (*)(int, int))function_map[i].func_ptr)(*(int *)params[0], *(int *)params[1]);
                printf("Result: %d\n", *(int *)result);
            } else if (function_map[i].return_size == sizeof(char)) {
                result = malloc(sizeof(char));
                *(char *)result = ((char (*)(char))function_map[i].func_ptr)(*(char *)params[0]);
                printf("Result: %c\n", *(char *)result);
            }

            for (int j = 0; j < 2; j++) {
                if (params[j] != NULL) {
                    free(params[j]);
                }
            }
            if (result != NULL) {
                free(result);
            }

            restore_registers_from_stack(); // Restore registers after invoking the function
            return;
        }
    }
    printf("Function not found.\n");
}



