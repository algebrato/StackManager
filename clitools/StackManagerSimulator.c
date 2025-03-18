#include "StackManager.h"
#include <stdio.h>


int main()
{
    initialize_register_stack();
    
    invoke_function("add");
    return 0;

}