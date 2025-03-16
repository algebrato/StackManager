#include <StackManager.h>
#include <stdio.h>

int main() {

    initialize_register_stack();

    #ifdef DEBUG
    print_register_stack();
    print_program_stack();
    #endif

    char function_name[50];
    printf("Enter the name of the function to invoke: ");
    scanf("%s", function_name);
    invoke_function(function_name);

    return 0;

}


