#include <stdio.h>
#include "lpt3.h"
#include "lpt3.h"
#include "queue.h"

bool test_cb(lpt3 code, void *udata) {
    return lpt3_simplex_level(code) < 5;
}

int main()
{
    printf("lpt3 and LPT3 test program\n");

    printf("sizeof(lpt3) = %zu bytes\n", sizeof(lpt3));
    printf("sizeof(lpt3) = %zu bytes\n", sizeof(lpt3));
    lpt3_tree * tree = lpt3_tree_new(1024);
    lpt3_tree_subdivide_while(tree, test_cb, NULL);
    lpt3_tree_print_stats(tree);

    double * coords = malloc(sizeof(double) * LPT3_DIM * lpt3_tree_vertex_count(tree));
    int * idxs = malloc(sizeof(int) * (LPT3_DIM+1) * lpt3_tree_leaf_count(tree));

    lpt3_tree_vertex_emit_coords(tree, coords);
    lpt3_tree_emit_idxs(tree, idxs);

    lpt3_tree_print_stats(tree);

    lpt3_tree_free(tree);

    return 0;
} 