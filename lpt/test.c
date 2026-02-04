#include <stdio.h>
#include "lpt2.h"
#include "lpt3.h"

void visit_cb(lpt2 code) {
    lpt2_print_simplex(code);
    printf("\n");
}

void subdivided_cb(lpt2 code) {
    printf("Subdivided: ");
    lpt2_print_simplex(code);
    printf("\n");
}

int main()
{
    printf("LPT2 and LPT3 test program\n");

    printf("sizeof(lpt2) = %zu bytes\n", sizeof(lpt2));
    printf("sizeof(lpt3) = %zu bytes\n", sizeof(lpt3));
    lpt2 code2;
    lpt2_init(&code2, 0);
    lpt2_tree * tree2 = lpt2_tree_new(0);
    lpt2_tree_visit_leaf(tree2, visit_cb);
    printf("Subdividing LPT2 root simplex\n");
    lpt2_print_simplex(code2);
    printf("\n");
    lpt2_tree_compat_bisect(tree2, code2, subdivided_cb);
    printf("LPT2 tree created and bisected\n");
    lpt2_tree_visit_leaf(tree2, visit_cb);
    lpt2_tree_free(tree2);
    
    return 0;
} 