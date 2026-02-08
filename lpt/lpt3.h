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

#define LPT(x) lpt3_##x
#define lpt lpt3
#include "lpt_inc.h"
#undef lpt
#undef LPT

#endif // LPT3_H