#include <stdio.h>
#include "lpt2.h"
#include "lpt3.h"

int main()
{
    printf("LPT2 and LPT3 test program\n");

    printf("sizeof(lpt2) = %zu bytes\n", sizeof(lpt2));
    printf("sizeof(lpt3) = %zu bytes\n", sizeof(lpt3));
    lpt2 code2;
    lpt2_init(&code2, 0);
    lpt2_print_simplex(code2);

    lpt3 code3;
    lpt3_init(&code3, 0);
    lpt3_print_simplex(code3);

    return 0;
} 