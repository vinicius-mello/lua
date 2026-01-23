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
    uint8_t valid : 1;
    uint8_t leaf : 1;
    uint8_t sigperm : 6;
    uint8_t level : 2;
    uint8_t orth_level : 6;
    uint16_t orth[3];
  } u;
};

typedef union lpt3_u lpt3;

#pragma pack(pop) // ok

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

#endif // LPT3_H