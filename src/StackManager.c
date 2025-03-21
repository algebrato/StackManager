#include "StackManager.h"
Register stack[STACK_SIZE];
Register registers[NUM_REGISTERS];
int stack_pointer = 0;
int register_pointer = 0;

/*
Questo e' il modo piu' semplice che mi e' venuto in mente per dividere un double in due registri
DoubleSplitter e' semplicemente una union di un double e di una struct che contiene due interi,
dato che il double e i due interi condividono la stessa area di memoria (tramite union), ho la sua 
rappresentazione in due interi, uno della parte "alta" e uno della parte "bassa".

Onestamente non so come una CPU a 32-bit lo faccia realmente.
*/
void split_double(double value, Register* high_reg, Register* low_reg) {
    DoubleSplitter splitter;
    splitter.d = value;
    high_reg->i = splitter.parts.high_bits; // No need for explicit casting
    low_reg->i = splitter.parts.low_bits;  // No need for explicit casting
}

/*
Stesso principio dello split_double, ma mappando una struct di due interi in un double.
*/
double combine_double(Register high_reg, Register low_reg) {
    DoubleSplitter splitter;
    splitter.parts.high_bits = high_reg.i;
    splitter.parts.low_bits = low_reg.i;
    return splitter.d;
}

/*
Il register bank e' un array di 8 registri, ognuno dei quali puo' contenere un intero, un float o un char.
In questo modo, quando ho bisogno di utilizzare un registro, lo inizializzo con il valore che mi serve e lo aggiungo
al register bank, per poi copiarlo sullo stack. In questo modo mi assicuro di utilizzare sempre un numero di registri 
fisso in accordo con la nostra architettura a 32-bit che stiamo simulando. (In questo caso 8 registri nel register_bank)
*/
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

/*
Stesso principio di add_register ma in senso inverso, estraggo sempre in modalita' LIFO il registro TOP per farne il
push sullo stack. Questa chiamata non fa il push sullo stack, ma restituisce il registro estratto. 
*/

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

/*
Questa funzione e' stata creata per debuggare il register bank, stampa il valore di un registro in base al tipo
*/
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

/*
Altra funzione di debug, stampa lo stato del register bank
*/
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


/*
Inizializzazione dei registri. Passando un valore random e assegandolo a ciascun registro.
sto effettivamente simulando una realizzazione random di 32 elementi. In questo caso, ogni registro
del register_bank viene inizializzato con un valore random. Quindi ho 8 registri, da 32-bit ciascuno,
inizializzato con un valore random. 
Dato che Register e' una union, in questo caso ho deciso di assegnare il valore random alla sua rappresentazione
integer. In funzione al tipo richiamato (reg.f, reg.i, reg.c) avro' la codifica di quel valore random in float, int o char.
*/
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

/*
Dato che consideriamo la realizzazione iniziale del register bank come una realizzazione "speciale",
questa helper function mi permette di salvare i valori dei registri sullo stack. Questa variante non 
modifica il register_pointer, quindi se vuoi brasare il regester bank, devi chiamare clear_registers().
*/
void save_registersBank_on_stack()
{
    for (int i = NUM_REGISTERS - 1; i >= 0; i--) {
        push(registers[i]);
        #ifdef DEBUG
        printf("Saved Register %d to stack\n", i);
        #endif
    }
}

/*
clean_registers() resetta il register_pointer a 0 permettendo di sovrascrivere i valori dei registri.
*/
void clear_registers() {
    register_pointer = 0;
    printf("All registers cleared\n");
}

/*
La realizzazione "speciale" iniziale del register bank viene ripristinata.
*/
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
/*Chiamate dedicate alla gestione dello stack                         */
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

/*
Se lo stack non e' pieno, inserisce nello stack un valore di tipo Register e lo stack_pointer 
viene incrementato
*/
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

/*
Se lo stack_pointer e' maggiore di 0, estrae il valore in cima allo stack e lo stack_pointer viene decrementato
*/
Register pop() {
    #ifdef DEBUG
    printf("\n=== Executing POP operation ===\n");
    #endif
    Register empty = { 0 };
    if (stack_pointer > 0) {
        Register reg = stack[--stack_pointer];
        #ifdef DEBUG
        printf("Popped value: %d (int) | %f (float) | %c (char)\n", reg.i, reg.f, reg.c);
        print_stack();
        #endif
        return reg;
    }
    
    fprintf(stderr, "Stack underflow\n");
    #ifdef DEBUG
    print_stack();
    #endif

    return empty;
}

/******************************************************************** */
/*Funzioni dedicate alla gestione delle operazioni                   */
/******************************************************************** */

/*
tutte le funzioni sono void f(void) perche' prendono i parametri dallo stack e 
restituiscono il risultato sullo stack.
*/
void add_double() {

    // Pop the first four registers from the stack
    Register param1_low = pop();
    Register param1_high = pop();
    
    Register param2_low = pop();
    Register param2_high = pop();
    
    /*
    Sicuramente queste righe sono sbagliate o comunque decisamente naive. Ho fatto l'eserizio di pensare
    ad un double su due registri, ma una volta che ho i 4 registri (due per il par1, 
    due per il par2) non so bene cosa succede all'interno della CPU per sommarli. Probabilmente
    result sara' il risultato di un processo iterativo che man mano raffina le cifre decimali e 
    poi le rappresenta su due registri. Ho provato a ragionare un po' sulla rappresentazione di un double
    con mantissa ed esponente, ma fare poi il bitset iterativo per riempire i due registri per il result va
    decisamente oltre le mie competenze attuali.
    */
    double d1 = combine_double(param1_high, param1_low);
    double d2 = combine_double(param2_high, param2_low);
    double result = d1 + d2;
    /************************************************ */

    // nuovamente, split_double e' una funzione che divide un double in due registri sempre in modo 
    // un po' naive. 
    Register result_high, result_low;
    split_double(result, &result_high, &result_low);
    push(result_high);
    push(result_low);

    return;
}

void multiply() {

    /*
    La moltipicazione e' semplice in questo caso. Dato che e' tra due interi, estraggo i due registri
    dallo stack e faccio la moltiplicazione iterativa.
    */
    Register a = pop();
    Register b = pop();

    int result = 0;
    while (b.i != 0) {
        if (b.i & 1) {
            result += a.i;
        }
        a.i <<= 1;
        b.i >>= 1;
    }

    /*Inserisco il registro sullo stack*/
    Register result_reg = { .i = result };
    push(result_reg);

    return;

}

void to_upper() {
    /*
    Funzione che prende un carattere minuscolo e lo trasforma in maiuscolo.
    */

    Register c = pop();
    if (c.c >= 'a' && c.c <= 'z') {
        c.c -= ('a' - 'A');
    }
    push(c);

    return;
}

/******************************************************************** */
/*Mappa per la gestione delle funzioni                                */
/******************************************************************** */
FunctionMap* create_function_map() {
    FunctionMap* map = malloc(sizeof(FunctionMap) * 3);

    // Funzione somma, nome, puntatore alla funzione, tipo e numero dei parametri e 
    // tipo del valore di ritorno
    map[0].name = "add";
    map[0].func_ptr = add_double;
    map[0].signature.param_types[0] = TYPE_DOUBLE;
    map[0].signature.param_types[1] = TYPE_DOUBLE;
    map[0].signature.param_count = 2;
    map[0].signature.return_type = TYPE_DOUBLE;


    map[1].name = "multiply";
    map[1].func_ptr = multiply;
    map[1].signature.param_types[0] = TYPE_INT;
    map[1].signature.param_types[1] = TYPE_INT;
    map[1].signature.param_count = 2;
    map[1].signature.return_type = TYPE_INT;

    map[2].name = "to_upper";
    map[2].func_ptr = to_upper;
    map[2].signature.param_types[0] = TYPE_CHAR;
    map[2].signature.param_count = 1;
    map[2].signature.return_type = TYPE_CHAR;

    return map;
}

void run_stack(FunctionMap *map) {
    do{
        /*
        Questa funzione cicla lo stack. Nella prima estrazione si aspetta l'identificativo di una funzione.
        Se l'identificativo e' 1, 2 o 3, viene eseguita la funzione corrispondente. Dalla mappa viene estratto
        il tipe del valore di ritorno. Viene eseguita la funzione, si fa l'allocazione di result per il tipo di ritorno, 
        e a result viene assegnato il registro in cima allo stack (che la funzione avra' riempito con il risultato).
        Al termine si fa un print a schermo.
        */
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

/*
Viene preparato uno stack aggiungendo nell'ordine:
- i valori dei parametri della funzione
- l'identificatore della funzione
Durante il pop dallo stack, l'identificativo della funzione e' il primo elemento
ad essere parsato, successivamente, se viene riconosciuto un id valito associato ad una funzione,
viene fatto il parsing dei parametri. Per questo motivo lo stack e' riempito in questo modo:

- parametro 2
- parametro 1
- identificativo della funzione

in modo da avere un ordine di estrazione corretto.
*/
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
            
            Register func_id = { .i = i+1 };
            push(func_id);
            return;

        }
    }
    fprintf(stderr, "Function not found: %s\n", func_name);
}
