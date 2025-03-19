#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

// Define register structure (32-bit)
typedef union {
    int i;     // 32-bit signed integer
    int u;    // 32-bit unsigned integer
    float f;       // 32-bit floating point
    char c;        // 8-bit character
    int d_parts;  // For storing parts of a double
} Register;        // Total size: 32 bits

// Helper union to split double into 32-bit parts
typedef union {
    double d;
    struct {
        int low_bits;
        int high_bits;
    } parts;
} DoubleSplitter;

// Function to combine two registers into a double
double combine_double(Register high_reg, Register low_reg) {
    DoubleSplitter splitter;
    splitter.parts.high_bits = high_reg.u;
    splitter.parts.low_bits = low_reg.u;
    return splitter.d;
}

// Function to split a double into two registers
void split_double(double value, Register* high_reg, Register* low_reg) {
    DoubleSplitter splitter;
    splitter.d = value;
    high_reg->u = splitter.parts.high_bits;
    low_reg->u = splitter.parts.low_bits;
}
// Define parameter types
typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_VOID
} ParamType;

// Structure to define function parameters
typedef struct {
    ParamType param_types[2];  // Array to store parameter types
    int param_count;           // Number of parameters
    ParamType return_type;     // Return type
} FunctionSignature;

// Structure for function mapping
typedef struct {
    const char* name;
    void* func_ptr;
    FunctionSignature signature;
} FunctionMap;

// Stack implementation
#define STACK_SIZE 256
Register stack[STACK_SIZE];
int stack_pointer = 0;

// Register bank
#define NUM_REGISTERS 8
Register registers[NUM_REGISTERS];

void initialize_registers() {
    // Seed the random number generator with current time
    srand((unsigned int)time(NULL));

    printf("\n=== Initializing Registers with Random Values ===\n");

    for (int i = 0; i < NUM_REGISTERS; i++) {
        // Generate a random 32-bit value
        registers[i].i = rand();

        printf("Register %d initialized:\n", i);
        printf("  As Integer: %d\n", registers[i].i);
        printf("  As Float: %f\n", registers[i].f);
        printf("  As Char: %c\n", registers[i].c);
    }
    printf("\n");
}

// Function to print the type of data stored in a Register
void print_register_value(Register reg, ParamType type) {
    switch (type) {
    case TYPE_INT:
        printf("%d", reg.i);
        break;
    case TYPE_DOUBLE:
        printf("0x%08x (part of double)", reg.u);
        break;
    case TYPE_CHAR:
        printf("%c", reg.c);
        break;
    default:
        printf("unknown");
    }
}

void print_registers() {
    printf("\nRegisters State:\n");
    for (int i = 0; i < NUM_REGISTERS; i += 2) {
        if (i + 1 < NUM_REGISTERS) {
            double d_val = combine_double(registers[i], registers[i + 1]);
            printf("R%d-R%d: %f (double) | ", i, i + 1, d_val);
        }
        printf("R%d: %d (int) | %f (float) | %c (char)\n",
            i, registers[i].i, registers[i].f, registers[i].c);
    }
    printf("\n");
}

void print_stack() {
    printf("\nStack State (SP = %d):\n", stack_pointer);
    if (stack_pointer == 0) {
        printf("Stack is empty\n");
    }
    else {
        for (int i = stack_pointer - 1; i >= 0; i--) {
            printf("[%d]: %d (int) | %f (float) | %c (char)\n",
                i, stack[i].i, stack[i].f, stack[i].c);
        }
    }
    printf("\n");
}

void push(Register value) {
    printf("\n=== Executing PUSH operation ===\n");
    if (stack_pointer < STACK_SIZE) {
        stack[stack_pointer++] = value;
        printf("Pushed value: %d (int) | %f (float) | %c (char)\n",
            value.i, value.f, value.c);
        print_stack();
    }
    else {
        fprintf(stderr, "Stack overflow\n");
    }
}

Register pop() {
    printf("\n=== Executing POP operation ===\n");
    Register empty = { 0 };
    if (stack_pointer > 0) {
        Register value = stack[--stack_pointer];
        printf("Popped value: %d (int) | %f (float) | %c (char)\n",
            value.i, value.f, value.c);
        print_stack();
        return value;
    }
    fprintf(stderr, "Stack underflow\n");
    print_stack();
    return empty;
}

// Function implementations
// Modified add function for doubles using two registers
double add_double(double a, double b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

char to_upper(char c) {
    return (c >= 'a' && c <= 'z') ? c - 32 : c;
}

// Initialize function map
FunctionMap* create_function_map() {
    FunctionMap* map = malloc(sizeof(FunctionMap) * 3);

    // Add function
    map[0].name = "add";
    map[0].func_ptr = add_double;
    map[0].signature.param_types[0] = TYPE_DOUBLE;
    map[0].signature.param_types[1] = TYPE_DOUBLE;
    map[0].signature.param_count = 2;
    map[0].signature.return_type = TYPE_DOUBLE;

    // Multiply function
    map[1].name = "multiply";
    map[1].func_ptr = multiply;
    map[1].signature.param_types[0] = TYPE_INT;
    map[1].signature.param_types[1] = TYPE_INT;
    map[1].signature.param_count = 2;
    map[1].signature.return_type = TYPE_INT;

    // ToUpper function
    map[2].name = "to_upper";
    map[2].func_ptr = to_upper;
    map[2].signature.param_types[0] = TYPE_CHAR;
    map[2].signature.param_count = 1;
    map[2].signature.return_type = TYPE_CHAR;

    return map;
}

void execute_function(const char* func_name, FunctionMap* map) {
    printf("\n=== Executing function: %s ===\n", func_name);
    printf("Stack state before function execution:\n");
    print_stack();

    for (int i = 0; i < 3; i++) {
        if (strcmp(map[i].name, func_name) == 0) {
            Register result;

            if (map[i].signature.param_count == 2) {
                Register param2 = pop();
                Register param1 = pop();

                printf("Parameters:\n");
                printf("  Param 1: ");
                print_register_value(param1, map[i].signature.param_types[0]);
                printf("\n  Param 2: ");
                print_register_value(param2, map[i].signature.param_types[1]);
                printf("\n");

                if (map[i].signature.param_types[0] == TYPE_DOUBLE) {
                    Register param2_high = pop();
                    Register param2_low = pop();
                    Register param1_high = pop();
                    Register param1_low = pop();

                    // Reconstruct doubles from register pairs
                    double param1 = combine_double(param1_high, param1_low);
                    double param2 = combine_double(param2_high, param2_low);

                    printf("Parameters:\n");
                    printf("  Param 1: %f\n", param1);
                    printf("  Param 2: %f\n", param2);

                    // Execute double precision function
                    double (*func_ptr)(double, double) = map[i].func_ptr;
                    double result_d = func_ptr(param1, param2);

                    // Split result back into two registers and push
                    Register result_high, result_low;
                    split_double(result_d, &result_high, &result_low);
                    push(result_low);   // Push low bits first
                    push(result_high);  // Then high bits
                }
                else if (map[i].signature.param_types[0] == TYPE_INT) {
                    int (*func_ptr)(int, int) = map[i].func_ptr;
                    result.i = func_ptr(param1.i, param2.i);
                    push(result);
                }
            }
            else if (map[i].signature.param_count == 1) {
                Register param = pop();
                printf("Parameters:\n");
                printf("  Param 1: ");
                print_register_value(param, map[i].signature.param_types[0]);
                printf("\n");

                if (map[i].signature.param_types[0] == TYPE_CHAR) {
                    char (*func_ptr)(char) = map[i].func_ptr;
                    result.c = func_ptr(param.c);
                    push(result);
                }
            }

            printf("\nStack state after function execution:\n");
            print_stack();
            return;
        }
    }
    fprintf(stderr, "Function not found: %s\n", func_name);
}

int main() {
    // Initialize registers with random values
    initialize_registers();

    FunctionMap* function_map = create_function_map();

    printf("\n=== Saving Initial Register Values to Stack ===\n");
    // Save all register values to stack
    for (int i = NUM_REGISTERS - 1; i >= 0; i--) {  // Push in reverse order for easier restoration
        push(registers[i]);
        printf("Saved Register %d to stack\n", i);
    }

    printf("\nInitial state before operations:\n");
    print_registers();
    print_stack();

    // Test add function with doubles
    printf("\nTesting ADD function with doubles:\n");
    double value1 = 3.14159265359;
    double value2 = 2.71828182846;

    // Split first double and push to stack
    Register value1_high, value1_low;
    split_double(value1, &value1_high, &value1_low);
    push(value1_low);   // Push low bits first
    push(value1_high);  // Then high bits

    // Split second double and push to stack
    Register value2_high, value2_low;
    split_double(value2, &value2_high, &value2_low);
    push(value2_low);
    push(value2_high);

    execute_function("add", function_map);

    // Pop result (comes in two parts)
    Register result_high = pop();
    Register result_low = pop();
    double result = combine_double(result_high, result_low);
    printf("Double Add result: %.10f\n", result);

    // Test multiply function
    printf("\nTesting MULTIPLY function:\n");
    Register a = { .i = 5 };
    Register b = { .i = 3 };
    push(a);
    push(b);
    execute_function("multiply", function_map);

    // Test to_upper function
    printf("\nTesting TO_UPPER function:\n");
    Register c = { .c = 'a' };
    push(c);
    execute_function("to_upper", function_map);

    printf("\n=== Restoring Initial Register Values from Stack ===\n");
    // Restore register values from stack
    for (int i = 0; i < NUM_REGISTERS; i++) {
        registers[i] = pop();
        printf("Restored Register %d from stack\n", i);
    }

    printf("\nFinal state after restoration:\n");
    print_registers();
    print_stack();

    free(function_map);
    return 0;
}
