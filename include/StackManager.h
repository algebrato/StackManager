#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h> 

// Stack size
#define STACK_SIZE 256
// Register bank [https://shivamrai24.medium.com/brief-about-cpu-register-1b532926e684]
#define NUM_REGISTERS 8

// Define register structure (32-bit)
typedef union {
    int i;     
    int u;    
    float f;       
    char c;        
    int d_parts;  
} Register;        

// Helper union to split double into 32-bit parts
typedef union {
    double d;
    struct {
        int low_bits;
        int high_bits;
    } parts;
} DoubleSplitter;

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
    void (*func_ptr)();
    FunctionSignature signature;
} FunctionMap;


double combine_double(Register high_reg, Register low_reg);
void split_double(double value, Register* high_reg, Register* low_reg);
void add_register(Register value);
Register remove_register();
void print_register_value(Register reg, ParamType type);
void print_registers();
void initialize_registers();
void save_registersBank_on_stack();
void clear_registers();
void restore_registers();

void print_stack();
void push(Register value);
Register pop();

FunctionMap* create_function_map();
void print_available_functions(FunctionMap* map);
void create_stack(const char* func_name, FunctionMap* map);
void run_stack(FunctionMap *map);

// Function prototypes
void add_double();
void multiply();
void to_upper();
