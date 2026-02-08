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

#define LPT(x) lpt2_##x
#define lpt lpt2
#include "lpt_inc.h"
#undef lpt
#undef LPT

#endif // LPT2_H