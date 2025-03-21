#include "StackManager.h"


// // Define register structure (32-bit)
// typedef union {
//     int i;     // 32-bit signed integer
//     int u;    // 32-bit unsigned integer
//     float f;       // 32-bit floating point
//     char c;        // 8-bit character
//     int d_parts;  // For storing parts of a double
// } Register;        // Total size: 32 bits

// // Helper union to split double into 32-bit parts
// typedef union {
//     double d;
//     struct {
//         int low_bits;
//         int high_bits;
//     } parts;
// } DoubleSplitter;

// // Function to combine two registers into a double using DoubleSplitter
// double combine_double(Register high_reg, Register low_reg) {
//     DoubleSplitter splitter;
//     splitter.parts.high_bits = high_reg.i;
//     splitter.parts.low_bits = low_reg.i;
//     return splitter.d;
// }

// // Function to split a double into two registers
// void split_double(double value, Register* high_reg, Register* low_reg) {
//     DoubleSplitter splitter;
//     splitter.d = value;
//     high_reg->i = splitter.parts.high_bits; // No need for explicit casting
//     low_reg->i = splitter.parts.low_bits;  // No need for explicit casting
// }
// // Define parameter types
// typedef enum {
//     TYPE_INT,
//     TYPE_DOUBLE,
//     TYPE_CHAR,
//     TYPE_VOID
// } ParamType;

// // Structure to define function parameters
// typedef struct {
//     ParamType param_types[2];  // Array to store parameter types
//     int param_count;           // Number of parameters
//     ParamType return_type;     // Return type
// } FunctionSignature;

// // Structure for function mapping
// typedef struct {
//     const char* name;
//     void (*func_ptr)();
//     FunctionSignature signature;
// } FunctionMap;

// // Stack implementation
// #define STACK_SIZE 256
// Register stack[STACK_SIZE];
// int stack_pointer = 0;

// // Register bank [https://shivamrai24.medium.com/brief-about-cpu-register-1b532926e684]
// #define NUM_REGISTERS 8
// Register registers[NUM_REGISTERS];  // Register bank
// int register_pointer = 0;          // Points to next available register position

// // Add a register
// void add_register(Register value) {
//     if (register_pointer < NUM_REGISTERS) {
//         registers[register_pointer++] = value;
//         #ifdef DEBUG
//         printf("Added register at position %d\n", register_pointer - 1);
//         #endif
//     } else {
//         fprintf(stderr, "Register bank is full\n");
//     }
// }

// // Remove a register
// Register remove_register() {
//     Register empty = {0};
//     if (register_pointer > 0) {
//         Register value = registers[--register_pointer];
//         printf("Removed register from position %d\n", register_pointer);
//         return value;
//     }
//     fprintf(stderr, "Register bank is empty\n");
//     return empty;
// }

// // Function to print the type of data stored in a Register
// void print_register_value(Register reg, ParamType type) {
//     switch (type) {
//     case TYPE_INT:
//         printf("%d", reg.i);
//         break;
//     case TYPE_DOUBLE:
//         printf("0x%08x (part of double)", reg.u);
//         break;
//     case TYPE_CHAR:
//         printf("%c", reg.c);
//         break;
//     default:
//         printf("unknown");
//     }
// }

// // Print register bank state
// void print_registers() {
//     #ifdef DEBUG
//     printf("\nRegister Bank State:\n");
//     if (register_pointer == 0) {
//         printf("Register bank is empty\n");
//     } else {
//         for (int i = 0; i < register_pointer; i++) {
//             printf("Register[%d]: %d (int) | %f (float) | %c (char)\n",
//                 i, registers[i].i, registers[i].f, registers[i].c);
//         }
//     }
//     printf("\n");
//     #endif
// }


// // Initialize registers
// void initialize_registers() {
//     register_pointer = 0;  // Reset pointer first
//     // Add some initial values
//     srand(time(NULL));
//     for (int i = 0; i < NUM_REGISTERS; i++) {
//         Register reg = {
//             .f = rand()
//         };
//         add_register(reg);   
//     }
// }

// // Clear all registers
// void clear_registers() {
//     register_pointer = 0;
//     printf("All registers cleared\n");
// }

// /******************************************************************** */

// Register registers[NUM_REGISTERS];

// void print_stack() {
//     #ifdef DEBUG
//     printf("\nStack State (SP = %d):\n", stack_pointer);
//     if (stack_pointer == 0) {
//         printf("Stack is empty\n");
//     }
//     else {
//         for (int i = stack_pointer - 1; i >= 0; i--) {
//             printf("[%d]: %d (int) | %f (float) | %c (char)\n",
//                 i, stack[i].i, stack[i].f, stack[i].c);
//         }
//     }
//     printf("\n");
//     #endif
// }

// void push(Register value) {
//     #ifdef DEBUG
//     printf("\n=== Executing PUSH operation ===\n");
//     #endif
//     if (stack_pointer < STACK_SIZE) {
//         stack[stack_pointer++] = value;
//         #ifdef DEBUG
//         printf("Pushed value: %d (int) | %f (float) | %c (char)\n", value.i, value.f, value.c);
//         print_stack();
//         #endif
//     }
//     else {
//         fprintf(stderr, "Stack overflow\n");
//     }
// }

// Register pop() {
//     #ifdef DEBUG
//     printf("\n=== Executing POP operation ===\n");
//     #endif
//     Register empty = { 0 };
//     if (stack_pointer > 0) {
//         Register value = stack[--stack_pointer];
//         #ifdef DEBUG
//         printf("Popped value: %d (int) | %f (float) | %c (char)\n", value.i, value.f, value.c);
//         print_stack();
//         #endif
//         return value;
//     }
//     fprintf(stderr, "Stack underflow\n");
//     print_stack();
//     return empty;
// }

// // Function implementations
// // Modified add function for doubles using two registers
// void add_double() {

//     // Pop the first four registers from the stack
//     Register param1_low = pop();
//     Register param1_high = pop();
    
//     Register param2_low = pop();
//     Register param2_high = pop();
    

//     // Combine param1_high and param1_low to create a double d1
//     double d1 = combine_double(param1_high, param1_low);

//     // Combine param2_high and param2_low to create a double d2
//     double d2 = combine_double(param2_high, param2_low);

//     // Perform the addition
//     double result = d1 + d2;

//     // Split the result into two registers and push them onto the stack
//     Register result_high, result_low;
//     split_double(result, &result_high, &result_low);
//     push(result_high);
//     push(result_low);

//     return;
// }

// void multiply() {

//     Register a = pop();
//     Register b = pop();


//     int result = 0;
//     while (b.i != 0) {
//         // If the least significant bit of b is set, add a to the result
//         if (b.i & 1) {
//             result += a.i;
//         }
//         // Shift a left by 1 (equivalent to multiplying by 2)
//         a.i <<= 1;
//         // Shift b right by 1 (equivalent to dividing by 2)
//         b.i >>= 1;
//     }
//     Register result_reg = { .i = result };
//     push(result_reg);

//     return;

// }

// void to_upper() {

//     Register c = pop();
//     if (c.c >= 'a' && c.c <= 'z') {
//         c.c -= ('a' - 'A');
//     }
//     push(c);

//     return;
// }

// // Initialize function map
// FunctionMap* create_function_map() {
//     FunctionMap* map = malloc(sizeof(FunctionMap) * 3);

//     // Add function
//     map[0].name = "add";
//     map[0].func_ptr = add_double;
//     map[0].signature.param_types[0] = TYPE_DOUBLE;
//     map[0].signature.param_types[1] = TYPE_DOUBLE;
//     map[0].signature.param_count = 2;
//     map[0].signature.return_type = TYPE_DOUBLE;

//     // Multiply function
//     map[1].name = "multiply";
//     map[1].func_ptr = multiply;
//     map[1].signature.param_types[0] = TYPE_INT;
//     map[1].signature.param_types[1] = TYPE_INT;
//     map[1].signature.param_count = 2;
//     map[1].signature.return_type = TYPE_INT;

//     // ToUpper function
//     map[2].name = "to_upper";
//     map[2].func_ptr = to_upper;
//     map[2].signature.param_types[0] = TYPE_CHAR;
//     map[2].signature.param_count = 1;
//     map[2].signature.return_type = TYPE_CHAR;

//     return map;
// }

// void run_stack(FunctionMap *map) {
//     do{
//         void *result = NULL;
//         Register func_id = pop();
//         if (func_id.i == 1 || func_id.i == 2 || func_id.i == 3) {
//             ((void (*)())map[func_id.i - 1].func_ptr)();
//             if(map[func_id.i - 1].signature.return_type == TYPE_INT) {
//                 result = malloc(sizeof(int));
//                 *(int *)result = pop().i;
//                 printf("%s result: %d\n", map[func_id.i-1].name, *(int *)result);
//             } else if (map[func_id.i - 1].signature.return_type == TYPE_DOUBLE) {
//                 Register low = pop();
//                 Register high = pop();
//                 double result_double = combine_double(high, low);
//                 result = malloc(sizeof(double));
//                 *(double *)result = result_double;
//                 printf("%s result: %f\n", map[func_id.i-1].name, *(double *)result);
//             } else if (map[func_id.i - 1].signature.return_type == TYPE_CHAR) {
//                 result = malloc(sizeof(char));
//                 *(char *)result = pop().c;
//                 printf("%s result: %c\n", map[func_id.i-1].name, *(char *)result);
//             }
  

//         } else {
//             printf("No more mapped function found.\n");
//             push(func_id);
//             break;
//         }
//     } while(1);
// }

// void print_available_functions(FunctionMap* map) {
//     printf("\nAvailable functions:\n");
//     printf("1. add (takes two doubles)\n");
//     printf("2. multiply (takes two integers)\n");
//     printf("3. to_upper (takes one character)\n");
//     printf("0. Exit\n");
// }

// void create_stack(const char* func_name, FunctionMap* map) {
//     for (int i = 0; i < 3; i++) {
//         if (strcmp(map[i].name, func_name) == 0) {
            

//             for (int j = 0; j < map[i].signature.param_count; j++) {
//                 if (map[i].signature.param_types[j] == TYPE_INT) {
//                     int value;
//                     printf("Enter parameter %d (int): ", j + 1);
//                     scanf("%d", &value);
//                     Register reg = { .i = value };
//                     push(reg);
//                 } else if (map[i].signature.param_types[j] == TYPE_DOUBLE) {
//                     double value;
//                     printf("Enter parameter %d (double): ", j + 1);
//                     scanf("%lf", &value);
//                     Register high, low;
//                     split_double(value, &high, &low);
//                     push(high);
//                     push(low);
//                 } else if (map[i].signature.param_types[j] == TYPE_CHAR) {
//                     char value;
//                     printf("Enter parameter %d (char): ", j + 1);
//                     getchar(); // Clear buffer
//                     scanf("%c", &value);
//                     Register reg = { .c = value };
//                     push(reg);
//                 }
//             }

//             /*insert the function identifier in the stack*/
//             Register func_id = { .i = i+1 };
//             push(func_id);
//             return;

//         }
//     }
//     fprintf(stderr, "Function not found: %s\n", func_name);
// }


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
    save_registersBank_on_stack();


    clear_registers();
    #ifdef DEBUG
    print_registers();
    print_stack();
    #endif

    char func_name[50];
    do {
        print_available_functions(function_map);
        printf("\nEnter function name or exit [const char* expected]: ");
        scanf("%s", &func_name);

        int found = 0;
        for (int i = 0; i < 3; i++) {
            if (strcmp(function_map[i].name, func_name) == 0) {
            found = 1;
            create_stack(func_name, function_map);
            }
        }

        if (strcmp("exit", func_name) == 0) {
            printf("\nExiting program...\n");
            break;
        }

        if (!found) {
            printf("\nInvalid function name. Please try again.\n");
        }

        print_registers();
        print_stack();

    } while(1);

    run_stack(function_map);

    printf("\n=== Restoring Initial Register Values from Stack ===\n");
    // Restore register values from stack
    restore_registers();


    printf("\nFinal state after restoration:\n");
    print_registers();
    print_stack();

    free(function_map);
    return 0;
}
