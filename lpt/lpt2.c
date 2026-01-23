#include <stdio.h>

#include "lpt2.h"

typedef uint8_t uchar;
typedef uint32_t uint;
typedef uint64_t ulong;
typedef uint16_t ushort;

#define DIM 2
#define MAX_ORTHANT_LEVEL 20

const int lpt2_table_sigma[8][2] = {
    {5, 2},
    {7, 3},
    {4, 0},
    {6, 1},
    {1, 6},
    {3, 7},
    {0, 4},
    {2, 5},
};

const int lpt2_table_sigma_inv[8][2] = {
    {6, 2},
    {4, 3},
    {7, 0},
    {5, 1},
    {2, 6},
    {0, 7},
    {3, 4},
    {1, 5},
};

const int lpt2_table_lft[8][2] = {
    {5, 2},
    {7, 3},
    {4, 0},
    {6, 1},
    {1, 6},
    {3, 7},
    {0, 4},
    {2, 5},
};

const int lpt2_table_rgt[8][2] = {
    {6, 2},
    {4, 3},
    {7, 0},
    {5, 1},
    {2, 6},
    {0, 7},
    {3, 4},
    {1, 5},
};

const int lpt2_table_swp[8][1] = {
    {4},
    {6},
    {5},
    {7},
    {0},
    {2},
    {1},
    {3},
};

const int lpt2_table_nsw[8][1] = {
    {7},
    {5},
    {6},
    {4},
    {3},
    {1},
    {2},
    {0},
};

const int lpt2_table_neg[8] = {1, 0, 3, 2, 5, 4, 7, 6};

const int lpt2_table_sigperm[8][2] = {
    {1, 2},
    {-1, 2},
    {1, -2},
    {-1, -2},
    {2, 1},
    {-2, 1},
    {2, -1},
    {-2, -1},
};

const int lpt2_table_inverse[8] = {0, 1, 2, 3, 4, 6, 5, 7};

const int lpt2_table_orth[8] = {0, 1, 2, 3, 0, 2, 1, 3};

void lpt2_init(lpt2 *code, uchar sigperm)
{
  code->u.level = 0;
  code->u.sigperm = sigperm<<DIM;
  code->u.orth_level = 0;
  code->u.orth_x = 0;
  code->u.orth_y = 0;
}

ulong lpt2_tointeger(lpt2 code)
{
  return code.code;
}

lpt2 lpt2_frominteger(ulong code)
{
  lpt2 r = {code};
  return r;
}

uchar lpt2_level_get(lpt2 code)
{
  return code.u.level;
}

void lpt2_level_set(lpt2 *code, uchar level)
{
  code->u.level = level;
}

uchar lpt2_sigperm_get(lpt2 code)
{
  return code.u.sigperm;
}

void lpt2_sigperm_set(lpt2 *code, uchar sigperm)
{
  code->u.sigperm = sigperm;
}

uchar lpt2_orthant_level_get(lpt2 code)
{
  return code.u.orth_level;
}

void lpt2_orthant_level_set(lpt2 *code, uchar orth_level)
{
  code->u.orth_level = orth_level;
}

uint lpt2_orthant_get(lpt2 code, int i)
{
  if (i == 0)
    return code.u.orth_x;
  else
    return code.u.orth_y;
}

void lpt2_orthant_set(lpt2 *code, int i, uint orth)
{
  if (i == 0)
    code->u.orth_x = orth;
  else
    code->u.orth_y = orth;
}

#define LPT(x) lpt2_##x
typedef lpt2 lpt;

#include "lpt_inc.c"