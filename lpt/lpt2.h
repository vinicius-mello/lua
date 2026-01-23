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
    uint8_t valid : 1;
    uint8_t leaf : 1;
    uint8_t sigperm : 6;
    uint8_t level : 2;
    uint8_t orth_level : 6;
    uint32_t orth_x : 20;
    uint32_t orth_y : 20;
  } u;
};

typedef union lpt2_u lpt2;

#pragma pack(pop) // ok

typedef union lpt2_u lpt2;

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

#endif // LPT2_H