#include "StackManager.h"
Register stack[STACK_SIZE];
Register registers[NUM_REGISTERS];
int stack_pointer = 0;
int register_pointer = 0;
// Function to combine two registers into a double using DoubleSplitter
double combine_double(Register high_reg, Register low_reg) {
    DoubleSplitter splitter;
    splitter.parts.high_bits = high_reg.i;
    splitter.parts.low_bits = low_reg.i;
    return splitter.d;
}

// Function to split a double into two registers
void split_double(double value, Register* high_reg, Register* low_reg) {
    DoubleSplitter splitter;
    splitter.d = value;
    high_reg->i = splitter.parts.high_bits; // No need for explicit casting
    low_reg->i = splitter.parts.low_bits;  // No need for explicit casting
}

// Add a register
void add_register(Register value) {
    if (register_pointer < NUM_REGISTERS) {
        registers[register_pointer++] = value;
        #ifdef DEBUG
        printf("Added register at position %d\n", register_pointer - 1);
        #endif
    } else {
        fprintf(stderr, "Register bank is full\n");
    }
}

// Remove a register
Register remove_register() {
    Register empty = {0};
    if (register_pointer > 0) {
        Register value = registers[--register_pointer];
        printf("Removed register from position %d\n", register_pointer);
        return value;
    }
    fprintf(stderr, "Register bank is empty\n");
    return empty;
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

// Print register bank state
void print_registers() {
    #ifdef DEBUG
    printf("\nRegister Bank State:\n");
    if (register_pointer == 0) {
        printf("Register bank is empty\n");
    } else {
        for (int i = 0; i < register_pointer; i++) {
            printf("Register[%d]: %d (int) | %f (float) | %c (char)\n",
                i, registers[i].i, registers[i].f, registers[i].c);
        }
    }
    printf("\n");
    #endif
}


// Initialize registers
void initialize_registers() {
    register_pointer = 0;  // Reset pointer first
    // Add some initial values
    srand(time(NULL));
    for (int i = 0; i < NUM_REGISTERS; i++) {
        Register reg = {
            .f = rand()
        };
        add_register(reg);   
    }
}

void save_registersBank_on_stack()
{
    for (int i = NUM_REGISTERS - 1; i >= 0; i--) {
        push(registers[i]);
        #ifdef DEBUG
        printf("Saved Register %d to stack\n", i);
        #endif
    }
}

// Clear all registers
void clear_registers() {
    register_pointer = 0;
    printf("All registers cleared\n");
}

void restore_registers()
{
    if ((stack_pointer) == NUM_REGISTERS) {

        for (int i = 0; i < NUM_REGISTERS; i++) {
            add_register(pop());
            printf("Restored Register %d from stack\n", i);
        }
    }
    else {
        fprintf(stderr, "Stack does not have enough values to restore registers\n");
    }
}

/******************************************************************** */

void print_stack() {
    #ifdef DEBUG
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
    #endif
}

void push(Register value) {
    #ifdef DEBUG
    printf("\n=== Executing PUSH operation ===\n");
    #endif
    if (stack_pointer < STACK_SIZE) {
        stack[stack_pointer++] = value;
        #ifdef DEBUG
        printf("Pushed value: %d (int) | %f (float) | %c (char)\n", value.i, value.f, value.c);
        print_stack();
        #endif
    }
    else {
        fprintf(stderr, "Stack overflow\n");
    }
}

Register pop() {
    #ifdef DEBUG
    printf("\n=== Executing POP operation ===\n");
    #endif
    Register empty = { 0 };
    if (stack_pointer > 0) {
        Register value = stack[--stack_pointer];
        #ifdef DEBUG
        printf("Popped value: %d (int) | %f (float) | %c (char)\n", value.i, value.f, value.c);
        print_stack();
        #endif
        return value;
    }
    fprintf(stderr, "Stack underflow\n");
    print_stack();
    return empty;
}

// Function implementations
// Modified add function for doubles using two registers
void add_double() {

    // Pop the first four registers from the stack
    Register param1_low = pop();
    Register param1_high = pop();
    
    Register param2_low = pop();
    Register param2_high = pop();
    

    // Combine param1_high and param1_low to create a double d1
    double d1 = combine_double(param1_high, param1_low);

    // Combine param2_high and param2_low to create a double d2
    double d2 = combine_double(param2_high, param2_low);

    // Perform the addition
    double result = d1 + d2;

    // Split the result into two registers and push them onto the stack
    Register result_high, result_low;
    split_double(result, &result_high, &result_low);
    push(result_high);
    push(result_low);

    return;
}

void multiply() {

    Register a = pop();
    Register b = pop();


    int result = 0;
    while (b.i != 0) {
        // If the least significant bit of b is set, add a to the result
        if (b.i & 1) {
            result += a.i;
        }
        // Shift a left by 1 (equivalent to multiplying by 2)
        a.i <<= 1;
        // Shift b right by 1 (equivalent to dividing by 2)
        b.i >>= 1;
    }
    Register result_reg = { .i = result };
    push(result_reg);

    return;

}

void to_upper() {

    Register c = pop();
    if (c.c >= 'a' && c.c <= 'z') {
        c.c -= ('a' - 'A');
    }
    push(c);

    return;
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

void run_stack(FunctionMap *map) {
    do{
        void *result = NULL;
        Register func_id = pop();
        if (func_id.i == 1 || func_id.i == 2 || func_id.i == 3) {
            ((void (*)())map[func_id.i - 1].func_ptr)();
            if(map[func_id.i - 1].signature.return_type == TYPE_INT) {
                result = malloc(sizeof(int));
                *(int *)result = pop().i;
                printf("%s result: %d\n", map[func_id.i-1].name, *(int *)result);
            } else if (map[func_id.i - 1].signature.return_type == TYPE_DOUBLE) {
                Register low = pop();
                Register high = pop();
                double result_double = combine_double(high, low);
                result = malloc(sizeof(double));
                *(double *)result = result_double;
                printf("%s result: %f\n", map[func_id.i-1].name, *(double *)result);
            } else if (map[func_id.i - 1].signature.return_type == TYPE_CHAR) {
                result = malloc(sizeof(char));
                *(char *)result = pop().c;
                printf("%s result: %c\n", map[func_id.i-1].name, *(char *)result);
            }
  

        } else {
            printf("No more mapped function found.\n");
            push(func_id);
            break;
        }
    } while(1);
}

void print_available_functions(FunctionMap* map) {
    printf("\nAvailable functions:\n");
    printf("1. add (takes two doubles)\n");
    printf("2. multiply (takes two integers)\n");
    printf("3. to_upper (takes one character)\n");
    printf("0. Exit\n");
}

void create_stack(const char* func_name, FunctionMap* map) {
    for (int i = 0; i < 3; i++) {
        if (strcmp(map[i].name, func_name) == 0) {
            

            for (int j = 0; j < map[i].signature.param_count; j++) {
                if (map[i].signature.param_types[j] == TYPE_INT) {
                    int value;
                    printf("Enter parameter %d (int): ", j + 1);
                    scanf("%d", &value);
                    Register reg = { .i = value };
                    push(reg);
                } else if (map[i].signature.param_types[j] == TYPE_DOUBLE) {
                    double value;
                    printf("Enter parameter %d (double): ", j + 1);
                    scanf("%lf", &value);
                    Register high, low;
                    split_double(value, &high, &low);
                    push(high);
                    push(low);
                } else if (map[i].signature.param_types[j] == TYPE_CHAR) {
                    char value;
                    printf("Enter parameter %d (char): ", j + 1);
                    getchar(); // Clear buffer
                    scanf("%c", &value);
                    Register reg = { .c = value };
                    push(reg);
                }
            }

            /*insert the function identifier in the stack*/
            Register func_id = { .i = i+1 };
            push(func_id);
            return;

        }
    }
    fprintf(stderr, "Function not found: %s\n", func_name);
}

