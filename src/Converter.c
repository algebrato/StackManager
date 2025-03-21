#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Function to convert char* to unsigned*
unsigned* convertCharToUnsigned(const char* input) {
    if (input == NULL) {
        fprintf(stderr, "Error: NULL input provided to convertCharToUnsigned.\n");
        return NULL;
    }

    char* endPtr;
    errno = 0;
    unsigned long value = strtoul(input, &endPtr, 10);

    if (errno != 0 || *endPtr != '\0') {
        fprintf(stderr, "Error: Invalid input '%s' for conversion to unsigned.\n", input);
        return NULL;
    }

    unsigned* result = (unsigned*)malloc(sizeof(unsigned));
    if (result == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    *result = (unsigned)value;
    return result;
}

// Function to convert int to unsigned*
unsigned* convertIntToUnsigned(int input) {
    unsigned* result = (unsigned*)malloc(sizeof(unsigned));
    if (result == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    *result = (unsigned)input;
    return result;
}

// Function to convert double to unsigned*
#ifndef UINT_MAX
#define UINT_MAX 255U // Maximum value for an unsigned int (8-bit)
#endif
unsigned* convertDoubleToUnsigned(double input) {
    if (input < 0 || input > (double)UINT_MAX) {
        fprintf(stderr, "Error: Input out of range for conversion to unsigned.\n");
        return NULL;
    }

    unsigned* result = (unsigned*)malloc(sizeof(unsigned));
    if (result == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    *result = (unsigned)input;
    return result;
}


// Function to print an unsigned value in binary format
void printUnsignedInBinary(unsigned value) {
    printf("Binary representation of %u: ", value);
    for (int i = sizeof(unsigned) * 8 - 1; i >= 0; i--) {
        printf("%u", (value >> i) & 1);
    }
    printf("\n");
}


