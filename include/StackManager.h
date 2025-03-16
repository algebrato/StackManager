#ifndef STACKMANAGER_H
#define STACKMANAGER_H

#include <stdint.h>
#include <stddef.h>

#define REGISTER_STACK_SIZE 32
#define PROGRAM_STACK_SIZE 1024

// Function prototypes
void initialize_register_stack();
void initialize_program_stack();
void print_register_stack();
void print_program_stack();
void invoke_function(const char *name);
void ciao(int a, int b);
double add(double x, double y);
int multiply(int x, int y);
char to_upper(char c);

typedef struct {
    const char *name;
    void (*func_ptr)();
    const char *param_types[2];
    const char *param_names[2];
    size_t return_size;
} FunctionInfo;

extern FunctionInfo function_map[];

#endif // STACKMANAGER_H