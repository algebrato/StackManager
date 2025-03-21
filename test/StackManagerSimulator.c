#include "StackManager.h"

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

    /*
    Dopo aver salvato i valori iniziali dei registri, sposto il register_pointer a 0
    e li vado a sovrascrivere.
    */
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

        #ifdef DEBUG
        print_registers();
        print_stack();
        #endif

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
