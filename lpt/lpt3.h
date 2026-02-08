#ifndef LPT3_H
#define LPT3_H

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push, 1) // force struct packing

union lpt3_u
{
  uint64_t code;
  struct
  {
    uint8_t reserved : 3;
    uint8_t orth_level : 5;
    uint8_t sigperm : 6;
    uint8_t level : 2;
    uint16_t orth[3];
  } u;
};

typedef union lpt3_u lpt3;

struct lpt3_tree_s;
typedef struct lpt3_tree_s lpt3_tree;

#pragma pack(pop) // ok

#define LPT3_DIM 3
#define LPT3_MAX_ORTHANT_LEVEL 16


void lpt3_init(lpt3 *code, uint8_t perm);
uint64_t lpt3_tointeger(lpt3 code);
lpt3 lpt3_frominteger(uint64_t code);
int lpt3_level(lpt3 code);
int lpt3_simplex_level(lpt3 code);
int lpt3_orthant_level(lpt3 code);
lpt3 lpt3_child(lpt3 code, int zo);
bool lpt3_is_child0(lpt3 code); 
lpt3 lpt3_parent(lpt3 code); 
int lpt3_orientation(lpt3 code);
int lpt3_neighbor_bd(lpt3 code, int i, lpt3 *r);
bool lpt3_neighbor(lpt3 code, int i, lpt3 *r);
void lpt3_simplex(lpt3 code, double *s);
void lpt3_print_simplex(lpt3 code);
lpt3_tree * lpt3_tree_new(size_t buckets);
void lpt3_tree_free(lpt3_tree *tree);
void lpt3_tree_print_stats(lpt3_tree *tree);
void lpt3_tree_compat_bisect(lpt3_tree *tree, lpt3 code, void (*subdivided)(lpt3,void*), void *udata);
bool lpt3_tree_is_leaf(lpt3_tree *tree, lpt3 code);
void lpt3_tree_visit_leafs(lpt3_tree *tree, void (*visit)(lpt3,void*), void *udata);
void lpt3_tree_search_all(lpt3_tree *tree, double * p, void (*visit)(lpt3,void*), void *udata);

#endif // LPT3_H