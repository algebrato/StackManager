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

// Function to combine two registers into a double using bitwise operations
double combine_double(Register high_reg, Register low_reg) {
    unsigned long long combined = ((unsigned long long)high_reg.u << 32) | (unsigned int)low_reg.u;
    double result;
    memcpy(&result, &combined, sizeof(double));
    return result;
}

// Function to split a double into two registers using bitwise operations
void split_double(double value, Register* high_reg, Register* low_reg) {
    unsigned long long combined;
    memcpy(&combined, &value, sizeof(double));
    high_reg->u = (unsigned int)(combined >> 32);
    low_reg->u = (unsigned int)(combined & 0xFFFFFFFF);
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

// Register bank [https://shivamrai24.medium.com/brief-about-cpu-register-1b532926e684]
#define NUM_REGISTERS 8
Register registers[NUM_REGISTERS];  // Register bank
int register_pointer = 0;          // Points to next available register position

// Add a register
void add_register(Register value) {
    if (register_pointer < NUM_REGISTERS) {
        registers[register_pointer++] = value;
        printf("Added register at position %d\n", register_pointer - 1);
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
}

// Initialize registers
void initialize_registers() {
    register_pointer = 0;  // Reset pointer first
    
    // Add some initial values
    for (int i = 0; i < NUM_REGISTERS; i++) {
        Register reg = {
            .c = 'A' + i
        };
        add_register(reg);
    }
}

// Clear all registers
void clear_registers() {
    register_pointer = 0;
    printf("All registers cleared\n");
}

/******************************************************************** */

Register registers[NUM_REGISTERS];

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
double add_double() {

    Register param1_high = pop();
    Register param1_low = pop();
    Register param2_high = pop();
    Register param2_low = pop();

    unsigned long long combined1 = ((unsigned long long)param1_high.u << 32) | (unsigned int)param1_low.u;
    unsigned long long combined2 = ((unsigned long long)param2_high.u << 32) | (unsigned int)param2_high.u;

    unsigned int carry;
    while (combined2 != 0) {
        // Calcola il carry
        carry = combined1 & combined2;

        // Esegui la somma senza il carry
        combined1 = combined1 ^ combined2;

        // Sposta il carry a sinistra di 1 bit
        combined2 = carry << 1;
    }

    double result;
    memcpy(&result, &combined1, sizeof(double));
    return result;
}

int multiply() {
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
    return result;
}

char to_upper() {
    Register c = pop();
    return (c.c >= 'a' && c.c <= 'z') ? c.c - 32 : c.c;
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
            double (*func_ptr)() = map[i].func_ptr;
            double result_d = func_ptr();


            printf("\nStack state after function execution:\n");
            print_stack();
            return;
        }
    }
    fprintf(stderr, "Function not found: %s\n", func_name);
}

void print_available_functions(FunctionMap* map) {
    printf("\nAvailable functions:\n");
    printf("1. add (takes two doubles)\n");
    printf("2. multiply (takes two integers)\n");
    printf("3. to_upper (takes one character)\n");
    printf("0. Exit\n");
}

int main() {
    /*
    Questo programma simula il comportamento delle operazioni macchina
    svolte da una CPU a 32-bit. Crea un assemby di istruzioni, vengono tradotte
    in binario e successivamente eseguite.

    L'architettura della CPU usata nella simulazione e' una versione MOLTO
    semplificata di un'architettua a 32-bit. Il register bank e' un array
    di 8 registri ognuno dei quali a 32-bit. La CPU-stack e' un array di 
    256 registri

    Al posto di usare un singolo registro da 32 elementi (come suggerito nel testo)
    viene usata un'array di registri in quanto con un singolo registro sarebbe stato 
    impossibile rappresentare un double. Il numero 8 di registri e' scelto perche' e'
    il numero di registri general purpose tipicamente usati in una CPU a 32-bit.
    [ref.https://shivamrai24.medium.com/brief-about-cpu-register-1b532926e684]
    */

    /*******************************************************************************/

    /*
    Il register bank viene inizializzato in modo random. All'interno di ognuno
    degli 8 registri viene inserito o un interno o un float o un char random.
    La CPU Stack, all'inizio e' vuota.
    */
    initialize_registers();

    /*
    La mappa delle funzioni e' un array di 3 elementi in cui ogni elemento
    e rappresentato da: [nome della funzione, la sua segnatura, il suo puntatore a funzione,
    e il tipo dei parametri e del valore di ritorno].
    */
    FunctionMap* function_map = create_function_map();

    /*
    Considero la realizzazione iniziale dei registri come una realizzazione "speciale"
    da voler ripristinare al termine dell'esecuzione del programma. Per questo motivo
    salvo i valori degli otto registri sullo stack.
    */
    printf("\n=== Saving Initial Register Values to Stack ===\n");
    // Save all register values to stack
    for (int i = NUM_REGISTERS - 1; i >= 0; i--) {
        push(registers[i]);
        printf("Saved Register %d to stack\n", i);
    }

    clear_registers();
    print_registers();
    print_stack();

    int choice;
    do {
        print_available_functions(function_map);
        printf("\nEnter function number (0 to exit): ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: { // add
                double value1, value2;
                printf("Enter first double value: ");
                scanf("%lf", &value1);
                printf("Enter second double value: ");
                scanf("%lf", &value2);

                // Split first double and push to stack
                Register value1_high, value1_low;
                Register value2_high, value2_low;

                split_double(value1, &value1_high, &value1_low);
                add_register(value1_high);
                add_register(value1_low);

                // Split second double and push to stack
                
                split_double(value2, &value2_high, &value2_low);
                add_register(value2_high);
                add_register(value2_low);

                push(remove_register());  // Push low bits first
                push(remove_register());  // Then high bits
                push(remove_register());  // Push low bits first
                push(remove_register());  // Then high bits

                execute_function("add", function_map);

                // Pop result (comes in two parts)
                Register result_high = pop();
                Register result_low = pop();
                double result = combine_double(result_high, result_low);
                printf("Double Add result: %.10f\n", result);
                break;
            }
            case 2: { // multiply
                int val1, val2;
                printf("Enter first integer: ");
                scanf("%d", &val1);
                printf("Enter second integer: ");
                scanf("%d", &val2);

                Register a = { .i = val1 };
                Register b = { .i = val2 };
                push(a);
                push(b);
                execute_function("multiply", function_map);

                Register result = pop();
                printf("Multiply result: %d\n", result.i);
                break;
            }
            case 3: { // to_upper
                char c;
                printf("Enter a character: ");
                getchar(); // Clear buffer
                scanf("%c", &c);

                Register chr = { .c = c };
                push(chr);
                execute_function("to_upper", function_map);
                Register result = pop();
                printf("ToUpper result: %c\n", result.c);
                break;
            }
            case 0:
                printf("\nExiting program...\n");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }

        if (choice != 0) {
            print_registers();
            print_stack();
        }

    } while (choice != 0);

    printf("\n=== Restoring Initial Register Values from Stack ===\n");
    // Restore register values from stack
    if ((stack_pointer) == NUM_REGISTERS) {

        for (int i = 0; i < NUM_REGISTERS; i++) {
            add_register(pop());
            printf("Restored Register %d from stack\n", i);
        }
    }
    else {
        fprintf(stderr, "Stack does not have enough values to restore registers\n");
    }

    printf("\nFinal state after restoration:\n");
    print_registers();
    print_stack();

    free(function_map);
    return 0;
}
