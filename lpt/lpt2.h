#ifndef LPT2_H
#define LPT2_H

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push, 1) // force struct packing

union lpt2_u
{
  uint64_t code;
  struct
  {
    uint8_t reserved : 3;
    uint8_t sigperm : 5;
    uint8_t level : 2;
    uint8_t orth_level : 6;
    uint32_t orth_x : 20;
    uint32_t orth_y : 20;
  } u;
};

typedef union lpt2_u lpt2;

#pragma pack(pop) // ok

#define LPT2_DIM 2
#define LPT2_MAX_ORTHANT_LEVEL 20


typedef union lpt2_u lpt2;

struct lpt2_tree_s;
typedef struct lpt2_tree_s lpt2_tree;

void lpt2_init(lpt2 *code, uint8_t perm);
uint64_t lpt2_tointeger(lpt2 code);
lpt2 lpt2_frominteger(uint64_t code);
int lpt2_level(lpt2 code);
int lpt2_simplex_level(lpt2 code);
int lpt2_orthant_level(lpt2 code);
lpt2 lpt2_child(lpt2 code, int zo);
bool lpt2_is_child0(lpt2 code); 
lpt2 lpt2_parent(lpt2 code); 
int lpt2_orientation(lpt2 code);
int lpt2_neighbor_bd(lpt2 code, int i, lpt2 *r);
bool lpt2_neighbor(lpt2 code, int i, lpt2 *r);
void lpt2_simplex(lpt2 code, double *s);
void lpt2_print_simplex(lpt2 code);
lpt2_tree * lpt2_tree_new(size_t buckets);
void lpt2_tree_free(lpt2_tree *tree);
void lpt2_tree_print_stats(lpt2_tree *tree);
void lpt2_tree_compat_bisect(lpt2_tree *tree, lpt2 code, void (*subdivided)(lpt2,void*), void *udata);
bool lpt2_tree_is_leaf(lpt2_tree *tree, lpt2 code);
void lpt2_tree_visit_leafs(lpt2_tree *tree, void (*visit)(lpt2,void*), void *udata);
void lpt2_tree_search_all(lpt2_tree *tree, double * p, void (*visit)(lpt2,void*), void *udata);

#endif // LPT2_H