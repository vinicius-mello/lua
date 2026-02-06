#include <stdio.h>
#include "lpt2.h"
#include "lpt3.h"

void visit_cb(lpt2 code, void *udata) {
    lpt2_print_simplex(code);
    printf("\n");
}

void subdivided_cb(lpt2 code, void *udata) {
    printf("Subdivided: ");
    lpt2_print_simplex(code);
    printf("\n");
}

int main()
{
    printf("LPT2 and LPT3 test program\n");

    printf("sizeof(lpt2) = %zu bytes\n", sizeof(lpt2));
    printf("sizeof(lpt3) = %zu bytes\n", sizeof(lpt3));
    lpt2_tree * tree2 = lpt2_tree_new(0);
    lpt2_tree_search_all(tree2, (double[]){0.0, 0.0}, visit_cb, NULL);
    lpt2_tree_visit_leafs(tree2, visit_cb, NULL);
    lpt2_tree_free(tree2);
    
    return 0;
} 