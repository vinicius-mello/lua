#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "morton.h"
#include "queue.h"

#define LPT_ABS(x) (((x) < 0) ? -(x) : (x))
#define LPT_SGN(x) (((x) < 0) ? -1 : 1)

int LPT(orth)(lpt code)
{
  return LPT(table_orth)[LPT(sigperm_get)(code)];
}

int LPT(sigma_inv)(lpt code, int l)
{
  return LPT(table_sigma_inv)[LPT(sigperm_get)(code)][l];
}

int LPT(sigma)(lpt code, int l)
{
  return LPT(table_sigma)[LPT(sigperm_get)(code)][l];
}

int LPT(lft)(lpt code, int l)
{
  return LPT(table_lft)[LPT(sigperm_get)(code)][l];
}

int LPT(rgt)(lpt code, int l)
{
  return LPT(table_rgt)[LPT(sigperm_get)(code)][l];
}

int LPT(sigperm)(lpt code, int i)
{
  return LPT(table_sigperm)[LPT(sigperm_get)(code)][i - 1];
}

int LPT(inverse)(lpt code)
{
  return LPT(table_inverse)[LPT(sigperm_get)(code)];
}

int LPT(neg)(lpt code)
{
  return LPT(table_neg)[LPT(sigperm_get)(code)];
}

int LPT(nsw)(lpt code, int l)
{
  return LPT(table_nsw)[LPT(sigperm_get)(code)][l - 1];
}

int LPT(swp)(lpt code, int l)
{
  return LPT(table_swp)[LPT(sigperm_get)(code)][l - 1];
}

bool LPT(is_root)(lpt code)
{
  return (LPT(level_get)(code) == 0 && LPT(orthant_level_get)(code) == 0);
}

int LPT(orientation)(lpt code)
{
  static const char ParityTable256[256] = // from Bit Twiddling Hacks
      {
#define LPT_P2(n) n, n ^ 1, n ^ 1, n
#define LPT_P4(n) LPT_P2(n), LPT_P2(n ^ 1), LPT_P2(n ^ 1), LPT_P2(n)
#define LPT_P6(n) LPT_P4(n), LPT_P4(n ^ 1), LPT_P4(n ^ 1), LPT_P4(n)
          LPT_P6(0), LPT_P6(1), LPT_P6(1), LPT_P6(0)};
  int m = (1 << DIM) - 1;
  char parsig = ParityTable256[LPT(sigperm_get)(code) & m];
  int perm = LPT(sigperm_get)(code) >> DIM;
  int parperm = ((perm & 2) >> 1) ^ (perm & 1); // in lex order,
  // permutation parity follows the pattern 0,1,1,0,...
  return (parperm ^ parsig) ? -1 : 1;
}

int LPT(level)(lpt code)
{
  return LPT(level_get)(code);
}

int LPT(orthant_level)(lpt code)
{
  return LPT(orthant_level_get)(code);
}

int LPT(simplex_level)(lpt code)
{
  return LPT(orthant_level_get)(code) * DIM + LPT(level_get)(code);
}

lpt LPT(child)(lpt code, int zo)
{
  lpt r = {0};
  LPT(level_set)(&r, (LPT(level_get)(code) + 1) % DIM);
  if (zo == 0)
    LPT(sigperm_set)(&r, LPT(sigperm_get)(code));
  else
    LPT(sigperm_set)(&r, LPT(sigma)(code, LPT(level_get)(code)));
  if (LPT(level_get)(r) == 0)
  {
    LPT(orthant_level_set)(&r, LPT(orthant_level_get)(code) + 1);
    int orth = LPT(orth)(r);
    for (int i = 0; i < DIM; ++i)
    {
      LPT(orthant_set)(&r, i, LPT(orthant_get)(code, i) << 1);
      if (orth & (1 << i))
        LPT(orthant_set)(&r, i, LPT(orthant_get)(r, i) | 1);
    }
  }
  else
  {
    LPT(orthant_level_set)(&r, LPT(orthant_level_get)(code));
    for (int i = 0; i < DIM; ++i)
      LPT(orthant_set)(&r, i, LPT(orthant_get)(code, i));
  }
  return r;
}

bool LPT(is_child0)(lpt code)
{
  if (LPT(orthant_level_get)(code) == 0)
  {
    if (LPT(level_get)(code) != 0)
    {
      int s = LPT(sigperm)(code, LPT(level_get)(code));
      return (s > 0);
    }
    else
      return false; // error
  }
  else if (LPT(orthant_level_get)(code) == 1)
  {
    if (LPT(level_get)(code) != 0)
    {
      int lstar = LPT(level_get)(code);
      int s = LPT(sigperm)(code, lstar);
      return LPT_SGN(s) == ((LPT(orthant_get)(code, LPT_ABS(s) - 1) & 1) ? -1 : 1);
    }
    else
    {
      int s = LPT(sigperm)(code, DIM);
      return (s > 0);
    }
  }
  else
  {
    int lstar = (LPT(level_get)(code) == 0) ? DIM : (LPT(level_get)(code));
    int s = LPT(sigperm)(code, lstar);
    if (LPT(level_get)(code) != 0)
      return LPT_SGN(s) == ((LPT(orthant_get)(code, LPT_ABS(s) - 1) & 1) ? -1 : 1);
    else
      return LPT_SGN(s) == ((LPT(orthant_get)(code, LPT_ABS(s) - 1) & 2) ? -1 : 1);
  }
}

lpt LPT(parent)(lpt code)
{
  lpt r = {0};
  if (LPT(level_get)(code) == 0)
  {
    LPT(level_set)(&r, DIM - 1);
    LPT(orthant_level_set)(&r, LPT(orthant_level_get)(code) - 1);
    for (int i = 0; i < DIM; ++i)
      LPT(orthant_set)(&r, i, LPT(orthant_get)(code, i) >> 1);
  }
  else
  {
    LPT(level_set)(&r, LPT(level_get)(code) - 1);
    LPT(orthant_level_set)(&r, LPT(orthant_level_get)(code));
    for (int i = 0; i < DIM; ++i)
      LPT(orthant_set)(&r, i, LPT(orthant_get)(code, i));
  }
  if (LPT(is_child0)(code))
  {
    LPT(sigperm_set)(&r, LPT(sigperm_get)(code));
  }
  else
  {
    LPT(sigperm_set)(&r, LPT(sigma_inv)(code, LPT(level_get)(r)));
  }
  return r;
}

int LPT(neighbor_bd)(lpt code, int i, lpt *r)
{
  *r = (lpt){0};
  LPT(level_set)(r, LPT(level_get)(code));
  LPT(orthant_level_set)(r, LPT(orthant_level_get)(code));
  if (LPT(is_child0)(code))
  {
    if (i == 0)
    {
      LPT(sigperm_set)(r, LPT(neg)(code));
    }
    else if (i == DIM)
    {
      int lminus = (LPT(level_get)(code) == 0) ? (DIM - 1) : (LPT(level_get)(code) - 1);
      LPT(sigperm_set)(r, LPT(lft)(code, lminus));
    }
    else
    {
      LPT(sigperm_set)(r, LPT(swp)(code, i));
    }
  }
  else
  {
    int lstar = (LPT(level_get)(code) == 0) ? (DIM) : (LPT(level_get)(code));
    if (i == 0)
    {
      LPT(sigperm_set)(r, LPT(neg)(code));
    }
    else if (i == lstar)
    {
      int lminus = (LPT(level_get)(code) == 0) ? (DIM - 1) : (LPT(level_get)(code) - 1);
      LPT(sigperm_set)(r, LPT(rgt)(code, lminus));
    }
    else if (i == DIM)
    {
      LPT(sigperm_set)(r, LPT(nsw)(code, LPT(level_get)(code)));
    }
    else
    {
      LPT(sigperm_set)(r, LPT(swp)(code, i));
    }
  }
  int p = LPT(sigperm)(code, 1);
  if (LPT(orthant_level_get)(code) == 0)
  {
    if (i == 0 || (LPT(level_get)(code) == 0 && i == DIM))
      return p;
    return 0;
  }
  if (i == 0)
  {
    for (int j = 0; j < DIM; ++j)
      LPT(orthant_set)(r, j, LPT(orthant_get)(code, j));
    int d = LPT_SGN(p);
    int x = LPT(orthant_get)(*r, LPT_ABS(p) - 1);
    int m;
    if (d == 1)
      m = 0;
    else
      m = (1 << LPT(orthant_level_get)(code)) - 1;
    if (x == m)
    {
      return p;
    }
    else
    {
      int m = (1 << LPT(orthant_level_get)(code)) - 1;
      if (d == 1)
        LPT(orthant_set)(r, LPT_ABS(p) - 1, m & (x - 1));
      else
        LPT(orthant_set)(r, LPT_ABS(p) - 1, m & (x + 1));
    }
  }
  else if (LPT(level_get)(code) == 0 && i == DIM)
  {
    int orth = LPT(orth)(*r);
    for (int j = 0; j < DIM; ++j)
    {
      LPT(orthant_set)(r, j, ((LPT(orthant_get)(code, j) >> 1) << 1));
      if (orth & (1 << j))
        LPT(orthant_set)(r, j, LPT(orthant_get)(*r, j) | 1);
    }
  }
  else
  {
    for (int j = 0; j < DIM; ++j)
      LPT(orthant_set)(r, j, LPT(orthant_get)(code, j));
  }
  return 0;
}

bool LPT(neighbor)(lpt code, int i, lpt *r)
{
  return LPT(neighbor_bd)(code, i, r) == 0;
}

void LPT(simplex)(lpt code, double *s)
{
  double t[DIM];
  for (int j = 0; j < DIM; ++j)
    t[j] = 0.0;
  for (int i = 0; i < LPT(orthant_level_get)(code); ++i)
  {
    for (int j = 0; j < DIM; ++j)
    {
      if (LPT(orthant_get)(code, j) & (1 << (LPT(orthant_level_get)(code) - 1 - i)))
        t[j] -= 1.0 / (2 << i);
      else
        t[j] += 1.0 / (2 << i);
    }
  }
  for (int i = 0; i <= DIM; ++i)
  {
    for (int j = 0; j < DIM; ++j)
    {
      double v;
      if (i > j)
        v = 1.0;
      else
      {
        if (i < LPT(level_get)(code))
          v = 0.0;
        else
          v = -1.0;
      }
      v = v / (1 << LPT(orthant_level_get)(code));
      int p = LPT(sigperm)(code, j + 1);
      s[i * DIM + (LPT_ABS(p) - 1)] = LPT_SGN(p) * v;
    }
    for (int j = 0; j < DIM; ++j)
    {
      s[i * DIM + j] += t[j];
    }
  }
}
//! Prints the simplex coordinates
void LPT(print_simplex)(lpt code)
{
  double s[DIM * (DIM + 1)];
  LPT(simplex)(code, s);
  printf("(");
  if(LPT(orientation)(code)<0) printf("-");
  else printf("+");
  for (int i = 0; i <= DIM; ++i)
  {
    printf("(");
    for (int j = 0; j < DIM; ++j)
    {
      printf("%g", s[i * DIM + j]);
      if (j != (DIM - 1))
        printf(",");
      else
        printf(")");
    }
  }
  printf(")");
}

#if 1
#define LPT_PRESENT_BIT 0x0000000000000001ll
#define LPT_LEAF_BIT    0x0000000000000002ll
#define LPT_BITS_MASK (~0x0000000000000007ll)
#else
#define LPT_PRESENT_BIT 0x8000000000000000ll
#define LPT_LEAF_BIT    0x4000000000000000ll
#define LPT_BITS_MASK (~0xE000000000000000ll)
#endif
#define LPT_VERTEX_PRESENT_BIT 0x0001

struct vertex_s {
  ulong mcode;
  ushort level;
  ushort flags; 
  int id;
};

typedef struct vertex_s vertex;

struct LPT(tree_s) {
  lpt * cell_slots;
  vertex * vert_slots;
  lpt_queue * cell_queue;
  size_t cell_buckets;
  size_t vert_buckets;
  size_t cell_collisions;
  size_t vert_collisions;
  size_t cells;
  size_t leaf_cells;
  size_t vertices;
};

typedef struct LPT(tree_s) LPT(tree);

uint64_t LPT(hash)(ulong code) {
  uint64_t x = LPT_BITS_MASK & code;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ll;
  x = (x ^ (x >> 27)) * 0x94d049b13c66a8edll;
  x = x ^ (x >> 31);
  return x;
}

bool LPT(tree_insert)(LPT(tree) *tree, lpt code, bool leaf);
bool LPT(tree_vertex_insert)(LPT(tree) *tree, ulong mcode);

LPT(tree) * LPT(tree_new)(size_t buckets)
{
  buckets = (buckets < 256) ? 256 : buckets;
  LPT(tree) * tree = (LPT(tree) *)malloc(sizeof(LPT(tree)));  
  tree->cell_slots = (lpt *)calloc(buckets, sizeof(lpt));
  tree->cells = 0;
  tree->leaf_cells = 0;
  tree->cell_buckets = buckets;
  tree->vert_slots = (vertex *)calloc(buckets, sizeof(vertex));
  tree->vertices = 0;
  tree->vert_buckets = buckets;
  tree->cell_collisions = 0;
  tree->vert_collisions = 0;
  tree->cell_queue = lpt_queue_new(256);
  lpt t = {0};
  static const int fact[5] = {1, 1, 2, 6, 24};
  for(int i=0;i<fact[DIM];++i) {
    LPT(init)(&t, i);
    LPT(tree_insert)(tree, t, true);
  }
  for(int i=0;i<(1<<DIM);++i) {
    double p[DIM];
    for(int j=0;j<DIM;++j) {
      p[j]=(i & (1<<j)) ? 1.0 : -1.0;
    }
    LPT(tree_vertex_insert)(tree, morton_encode(DIM,p));
  }
  return tree;
}

void LPT(tree_print_stats)(LPT(tree) *tree)
{
  printf("LPT Tree stats:\n");
  printf("  Cell Buckets: %zu\n", tree->cell_buckets);
  printf("  Cells: %zu\n", tree->cells);
  printf("  Leaf Cells: %zu\n", tree->leaf_cells);
  printf("  Cell Load factor: %.2f\n", (float)(tree->cells) / (float)(tree->cell_buckets));
  printf("  Cell Collisions: %zu\n", tree->cell_collisions);
  float n, m;
  n = (float)(tree->cells);
  m = (float)(tree->cell_buckets);
  float expected_cell_collisions = n-m*(1.0-pow(1.0-1.0/m, n));
  printf("  Expected Cell Collisions: %.2f\n", expected_cell_collisions);
  printf("  Vertex Buckets: %zu\n", tree->vert_buckets);
  printf("  Vertices: %zu\n", tree->vertices);
  printf("  Vertices Load factor: %.2f\n", (float)(tree->vertices) / (float)(tree->vert_buckets));
  n = (float)(tree->vertices);
  m = (float)(tree->vert_buckets);
  float expected_vert_collisions = n-m*(1.0-pow(1.0-1.0/m, n));
  printf("  Expected Vertex Collisions: %.2f\n", expected_vert_collisions);
  printf("  Vertex Collisions: %zu\n", tree->vert_collisions);
}

void LPT(tree_free)(LPT(tree) *tree)
{
  free(tree->cell_slots);
  free(tree->vert_slots);
  lpt_queue_free(tree->cell_queue);
  free(tree);
}

void LPT(tree_rehash)(LPT(tree) *tree, size_t new_buckets);

bool LPT(tree_insert)(LPT(tree) *tree, lpt code, bool leaf)
{
  float load_factor = (float)(tree->cells) / (float)(tree->cell_buckets);
  if (load_factor > 0.7f)
    LPT(tree_rehash)(tree, tree->cell_buckets * 2);
  size_t hash = (LPT(hash)(code.code)) % tree->cell_buckets;
  while ((tree->cell_slots[hash].code & LPT_PRESENT_BIT) != 0)
  {
    if ((tree->cell_slots[hash].code & LPT_BITS_MASK)
         == (code.code & LPT_BITS_MASK)) {
      return false; // already present
    }
    tree->cell_collisions++;
    hash = (hash + 1) % tree->cell_buckets;
  }
  tree->cell_slots[hash].code = code.code 
    | (leaf ? LPT_LEAF_BIT : 0) 
    | LPT_PRESENT_BIT;
  tree->cells++;
  if(leaf) tree->leaf_cells++;
  return true;
}

void LPT(tree_vertex_rehash)(LPT(tree) *tree, size_t new_buckets);

bool LPT(tree_vertex_insert)(LPT(tree) *tree, ulong mcode) {
  float load_factor = (float)(tree->vertices) / (float)(tree->vert_buckets);
  if (load_factor > 0.7f)
    LPT(tree_vertex_rehash)(tree, tree->vert_buckets * 2);
  size_t hash = LPT(hash)(mcode) % tree->vert_buckets;
  while((tree->vert_slots[hash].flags & LPT_VERTEX_PRESENT_BIT) != 0) {
    if(tree->vert_slots[hash].mcode == mcode) {
      return false; // already present
    }
    tree->vert_collisions++;
    hash = (hash + 1) % tree->vert_buckets;
  }
  tree->vert_slots[hash].mcode = mcode;
  tree->vert_slots[hash].flags |= LPT_VERTEX_PRESENT_BIT;
  tree->vert_slots[hash].id = tree->vertices;
  tree->vertices++;
  return true;
}

int LPT(tree_vertex_find_id)(LPT(tree) *tree, ulong mcode) {
  size_t hash = LPT(hash)(mcode) % tree->vert_buckets;
  while((tree->vert_slots[hash].flags & LPT_VERTEX_PRESENT_BIT) != 0) {
    if(tree->vert_slots[hash].mcode == mcode) {
      return tree->vert_slots[hash].id;
    }
    hash = (hash + 1) % tree->vert_buckets;
  }
  return -1;
}

int LPT(tree_find)(LPT(tree) *tree, lpt code)
{
  size_t hash = (LPT(hash)(code.code)) % tree->cell_buckets;
  while ((tree->cell_slots[hash].code & LPT_PRESENT_BIT) != 0)
  {
    if ((tree->cell_slots[hash].code & LPT_BITS_MASK) 
        == (code.code & LPT_BITS_MASK)) {
      return (int)hash;
    }
    hash = (hash + 1) % tree->cell_buckets;
  }
  return -1; // not found
}

void LPT(tree_rehash)(LPT(tree) *tree, size_t new_buckets)
{
  lpt *old_slots = tree->cell_slots;
  size_t old_buckets = tree->cell_buckets;
  tree->cell_slots = (lpt *)calloc(new_buckets, sizeof(lpt));
  tree->cell_buckets = new_buckets;
  tree->cells = 0;
  tree->leaf_cells = 0;
  tree->cell_collisions = 0;
  for (size_t i = 0; i < old_buckets; ++i)
  {
    if ((old_slots[i].code & LPT_PRESENT_BIT) != 0)
    {
      bool leaf = (old_slots[i].code & LPT_LEAF_BIT) != 0;
      lpt code = {old_slots[i].code & LPT_BITS_MASK};
      LPT(tree_insert)(tree, code, leaf);
    }
  }
  free(old_slots);
}

void LPT(tree_vertex_rehash)(LPT(tree) *tree, size_t new_buckets) {
  vertex *old_slots = tree->vert_slots;
  size_t old_buckets = tree->vert_buckets;
  tree->vert_slots = (vertex *)calloc(new_buckets, sizeof(vertex));
  tree->vert_buckets = new_buckets;
  tree->vertices = 0;
  tree->vert_collisions = 0;
  for (size_t i = 0; i < old_buckets; ++i)
  {
    if ((old_slots[i].flags & LPT_VERTEX_PRESENT_BIT) != 0)
    {
      ulong mcode = old_slots[i].mcode;
      LPT(tree_vertex_insert)(tree, mcode);
    }
  }
  free(old_slots);
}

int LPT(tree_vertex_count)(LPT(tree) *tree) {
  return tree->vertices;
}

int LPT(tree_leaf_count)(LPT(tree) *tree) {
  return tree->leaf_cells;
}

void LPT(tree_mark)(LPT(tree) *tree, lpt code)
{
  lpt_queue_pushright(tree->cell_queue, code.code);
}

void LPT(tree_unmark_all)(LPT(tree) *tree) 
{
  lpt_queue_reset(tree->cell_queue);
}

bool LPT(tree_is_marked)(LPT(tree) *tree, lpt code)
{
  return lpt_queue_contains(tree->cell_queue, code.code);
}

void LPT(tree_leaf)(LPT(tree) *tree, lpt code)
{
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    tree->cell_slots[index].code |= LPT_LEAF_BIT;
    tree->leaf_cells += 1; 
  } else {
    fprintf(stderr, "tree_leaf: code not found\n");
  }
}

void LPT(tree_unleaf)(LPT(tree) *tree, lpt code)
{
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    tree->cell_slots[index].code &= ~LPT_LEAF_BIT;
    tree->leaf_cells -= 1;
  } else {
    fprintf(stderr, "tree_unleaf: code not found\n");
  } 
}

bool LPT(tree_is_leaf)(LPT(tree) *tree, lpt code)
{
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    return (tree->cell_slots[index].code & LPT_LEAF_BIT) != 0;
  }   
  fprintf(stderr, "tree_is_leaf: code not found\n");
  return false;
}

bool LPT(tree_exists)(LPT(tree) *tree, lpt code)
{
  int index = LPT(tree_find)(tree, code);
  return index >= 0;
}

void LPT(tree_simple_bisect)(LPT(tree) *tree, lpt code)
{
  LPT(tree_unleaf)(tree, code);
  lpt c0 = LPT(child)(code, 0);
  lpt c1 = LPT(child)(code, 1);
  LPT(tree_insert)(tree, c0, true);
  LPT(tree_insert)(tree, c1, true);
}

void LPT(tree_compat_bisect_rec)(LPT(tree) *tree, lpt code,
  void (*subdivided)(lpt,void*),
  void (*new_vertex)(int,double *,void*), void *udata)
{
  LPT(tree_mark)(tree, code);
  for(int i=0;i<=DIM;++i) {
    if(i==DIM || i==LPT(level)(code)) continue;
    lpt n = {0};
    if(LPT(neighbor)(code, i, &n)) {
      if(!LPT(tree_exists)(tree, n)) {
        lpt p = LPT(parent)(n);
        LPT(tree_compat_bisect_rec)(tree, p, subdivided, new_vertex, udata);
      }
      if(LPT(tree_is_leaf)(tree, n) && !LPT(tree_is_marked)(tree, n)) {
        LPT(tree_compat_bisect_rec)(tree, n, subdivided, new_vertex, udata);
      }
    }
  }
  int l = LPT(level)(code);
  double s[DIM + 1][DIM];
  LPT(simplex)(LPT(child)(code, 0), &s[0][0]);
  ulong mcode = morton_encode(DIM, &s[l][0]);
  if(LPT(tree_vertex_insert)(tree, mcode)) {
    int vid = LPT(tree_vertex_find_id)(tree, mcode);
    if(new_vertex) new_vertex(vid, &s[l][0], udata);
  }
  LPT(tree_simple_bisect)(tree, code);
  if(subdivided) subdivided(code, udata);
}

void LPT(tree_compat_bisect)(LPT(tree) *tree, lpt code,
  void (*subdivided)(lpt,void*), void (*new_vertex)(int,double *,void*), void *udata) {
  LPT(tree_unmark_all)(tree);
  LPT(tree_compat_bisect_rec)(tree, code, subdivided, new_vertex, udata);
}

int LPT(tree_neighbor_bd)(LPT(tree) *tree, lpt r, int i, lpt *n) {
  int l=LPT(level)(r);
  int p=LPT(neighbor_bd)(r, i, n);
  if(p==0) {
    if(i==DIM) {
      lpt n0 = LPT(child)(*n, 0);
      lpt r1 = LPT(child)(r, 1);
      if(LPT(tree_exists)(tree, n0)) LPT(neighbor_bd)(r1, l, n);
    } else if(i==l) {
      lpt n0 = LPT(child)(*n, 0);
      lpt r0 = LPT(child)(r, 0);
      if(LPT(tree_exists)(tree, n0)) LPT(neighbor_bd)(r0, l, n);
    } else {
      if(!LPT(tree_exists)(tree, *n)) *n=LPT(parent)(*n);
    }
  }
  return p;
}
 
bool LPT(tree_neighbor)(LPT(tree) *tree, lpt r, int i, lpt *n) {
  return LPT(tree_neighbor_bd)(tree, r, i,n)==0;
}

int LPT(tree_neighbor_index)(LPT(tree) *tree, lpt r, lpt n) {
  for(int j=0;j<=DIM;++j) {
    lpt rr = {0};
    if(LPT(tree_neighbor)(tree, n, j, &rr)) {
      if((rr.code & LPT_BITS_MASK) == (r.code & LPT_BITS_MASK))
        return j;
    }
  }
  return -1;
}

lpt LPT(find_root)(double * p, double * w) {
  int a[DIM];
  static const int fact[5]={1,1,2,6,24};
  int key,i;
  for(i=0;i<DIM;++i) a[i]=i;
  for(int j=1;j<DIM;++j) {  //insertion sort
    key=a[j];
    i=j-1;
    while(i>=0 && p[a[i]]<p[key]) { //descending order
      a[i+1]=a[i];
      i--;
    }
    a[i+1]=key;
  }
  int code=0;
  for(i=0;i<DIM;++i) { // Lehmer code
    int k=0;
    for(int j=i+1;j<DIM;++j) if(a[j]<a[i]) ++k;
    code+=fact[DIM-i-1]*k;
  }
  lpt r = {0};
  LPT(init)(&r, code);
  w[0]=(1.0-p[a[1-1]])/2.0;
  for(i=1;i<DIM;++i) w[i]=(p[a[i-1]]-p[a[(i+1)-1]])/2.0;
  w[DIM]=(p[a[DIM-1]]+1.0)/2.0;
  return r;
}

lpt LPT(tree_search_rec)(LPT(tree) *tree, double * p, lpt r, double * w) {
  
  if(LPT(tree_is_leaf)(tree, r)) {
    return r;
  }
  int l=LPT(level)(r);
  if(w[l]<=w[DIM]) {
    double t=w[l];
    w[l]=2.0*w[l];
    w[DIM]=w[DIM]-t;
    return LPT(tree_search_rec)(tree, p, LPT(child)(r, 0), w);
  } else {
    double t=w[DIM];
    w[l]=w[l]-t;
    for(int i=DIM;i>l;--i) w[i]=w[i-1];
    w[l]=2.0*t;
    return LPT(tree_search_rec)(tree, p, LPT(child)(r, 1), w);
  }
}

lpt LPT(tree_search_w)(LPT(tree) *tree, double * p, double * w) {
  lpt r=LPT(find_root)(p,w);
  lpt result = LPT(tree_search_rec)(tree, p, r, w);
  return result;
}

lpt LPT(tree_search)(LPT(tree) *tree, double * p) {
  double w[DIM+1];
  lpt r=LPT(tree_search_w)(tree, p, w);
  return r; 
}

#define delete_bit(byte, i) (((byte>>1)&((~0u)<<i))|((~((~0u)<<i))&byte))
#define insert_bit(byte, i) (((((~0u)<<i)&byte)<<1)|((~((~0u)<<i))&byte))

void LPT(tree_search_all_rec)(LPT(tree) *tree, lpt r, unsigned int faces, void (*visit)(lpt,void*),void *udata) {
  LPT(tree_mark)(tree, r);
  for(unsigned int i=0;i<=DIM;++i) {
    if((faces & (1<<i))!=0) continue;
    lpt n = {0};
    if(LPT(tree_neighbor)(tree, r, i, &n)) {
      if(!LPT(tree_is_marked)(tree, n)) {
        int k = LPT(tree_neighbor_index)(tree, r, n);
        unsigned int new_faces =
          insert_bit(delete_bit(faces,i), k);
        LPT(tree_search_all_rec)(tree, n, new_faces, visit, udata);
      }
    }
  }
  visit(r, udata);
}

void LPT(tree_search_all)(LPT(tree) *tree, double * p, void (*visit)(lpt,void*), void *udata) {
  double w[DIM+1];
  lpt r=LPT(tree_search_w)(tree, p, w);
  unsigned int faces=0;
  for(int i=0;i<=DIM;++i) if(w[i]!=0.0) faces+= (1<<i);
  LPT(tree_unmark_all)(tree);
  LPT(tree_search_all_rec)(tree, r, faces, visit, udata);
}

void LPT(tree_visit_leafs)(LPT(tree) *tree, void (*visit)(lpt,void*), void *udata)
{
  for(size_t i=0;i<tree->cell_buckets;++i) {
    if(tree->cell_slots[i].code & (LPT_LEAF_BIT)) {
      lpt code = {tree->cell_slots[i].code & LPT_BITS_MASK};
      visit(code, udata);
    }
  }
}

void LPT(tree_vertex_emit_coords)(LPT(tree) *tree, double * coords) {
  for(size_t i=0;i<tree->vert_buckets;++i) {
    vertex v = tree->vert_slots[i];
    if((v.flags & LPT_VERTEX_PRESENT_BIT) != 0) {
      morton_decode(v.mcode, DIM, &coords[DIM*v.id]);
    }
  }
}

void LPT(tree_emit_idxs)(LPT(tree) *tree, int * idxs) {
  size_t k = 0;
  for(size_t i=0;i<tree->cell_buckets;++i) {
    lpt slot = tree->cell_slots[i];
    if(slot.code & (LPT_LEAF_BIT)) { 
      lpt code = {slot.code & LPT_BITS_MASK}; 
      double s[DIM + 1][DIM]; 
      LPT(simplex)(code, &s[0][0]); 
      for(int j=0;j<=DIM;++j) {
        ulong mcode = morton_encode(DIM, &s[j][0]);
        int vid = LPT(tree_vertex_find_id)(tree, mcode);
        idxs[(DIM+1)*(k)+ j] = vid;
      }
      k++;
    }
  }
}

void LPT(subdivide_cb)(lpt code, void *udata) {
  lpt_queue *q = (lpt_queue *)udata;
  lpt_queue_pushright(q, LPT(tointeger)(LPT(child)(code, 0)));
  lpt_queue_pushright(q, LPT(tointeger)(LPT(child)(code, 1)));
}

void LPT(tree_subdivide_until)(LPT(tree) *tree, bool (*test)(lpt, void*), void *udata) {
  lpt_queue * q = lpt_queue_new(256);
  for(size_t i=0;i<tree->cell_buckets;++i) {
    lpt slot = tree->cell_slots[i];
    if(slot.code & (LPT_LEAF_BIT)) {
      lpt code = {slot.code & LPT_BITS_MASK};
      lpt_queue_pushright(q, code.code);
    }
  }
  while(!lpt_queue_empty(q)) {
    lpt code = {lpt_queue_popleft(q)};
    if(LPT(tree_is_leaf(tree, code))&&(!test(code, udata))) {
      LPT(tree_compat_bisect)(tree, code, LPT(subdivide_cb), NULL, q);
    }
  }
}