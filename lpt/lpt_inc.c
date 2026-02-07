#include <string.h>
#include <stdlib.h>

#define LPT_ABS(x) (((x) < 0) ? -(x) : (x))
#define LPT_SGN(x) (((x) < 0) ? -1 : 1)

#ifdef DEBUG
#define debug(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...) ((void)0)
#endif

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
  lpt r;
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
  lpt r;
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
#define LPT_MARK_BIT    0x0000000000000004ll
#define LPT_BITS_MASK (~0x0000000000000007ll)
#else
#define LPT_PRESENT_BIT 0x8000000000000000ll
#define LPT_LEAF_BIT    0x4000000000000000ll
#define LPT_MARK_BIT    0x2000000000000000ll
#define LPT_BITS_MASK (~0xE000000000000000ll)
#endif

struct LPT(tree_s) {
  lpt * slots;
  size_t elements;
  size_t buckets;
};
typedef struct LPT(tree_s) LPT(tree);

uint64_t LPT(hash)(lpt code) {
  uint64_t x = LPT_BITS_MASK & code.code;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ll;
  x = (x ^ (x >> 27)) * 0x94d049b13c66a8edll;
  x = x ^ (x >> 31);
  return x;
}

bool LPT(tree_insert)(LPT(tree) *tree, lpt code, bool leaf, bool mark);

LPT(tree) * LPT(tree_new)(size_t buckets)
{
  debug("enter tree_new: buckets: %zu\n", buckets);
  buckets = (buckets < 16) ? 16 : buckets;
  LPT(tree) * tree = (LPT(tree) *)malloc(sizeof(LPT(tree)));  
  tree->slots = (lpt *)calloc(buckets, sizeof(lpt));
  tree->elements = 0;
  tree->buckets = buckets;
  lpt t;
  static const int fact[5] = {1, 1, 2, 6, 24};
  for(int i=0;i<fact[DIM];++i) {
    LPT(init)(&t, i);
    LPT(tree_insert)(tree, t, true, false);
  }
  debug("exit tree_new\n");
  return tree;
}

void LPT(tree_print_stats)(LPT(tree) *tree)
{
  size_t used = 0;
  size_t leaf = 0;
  for (size_t i = 0; i < tree->buckets; ++i)
  {
    if ((tree->slots[i].code & LPT_PRESENT_BIT) != 0)
      used++;
    if ((tree->slots[i].code & LPT_LEAF_BIT) != 0)
      leaf++;
  }
  printf("LPT Tree stats:\n");
  printf("  Buckets: %zu\n", tree->buckets);
  printf("  Elements: %zu\n", tree->elements);
  printf("  Used slots: %zu\n", used);
  printf("  Leaf slots: %zu\n", leaf);
  printf("  Load factor: %.2f\n", (float)(tree->elements) / (float)(tree->buckets));
}

void LPT(tree_free)(LPT(tree) *tree)
{
  debug("enter tree_free\n");
  free(tree->slots);
  free(tree);
  debug("exit tree_free\n");
}

void LPT(tree_rehash)(LPT(tree) *tree, size_t new_buckets);

bool LPT(tree_insert)(LPT(tree) *tree, lpt code, bool leaf, bool mark)
{
  debug("enter tree_insert: code %016lx, leaf %d, mark %d\n", code.code, leaf, mark);
  float load_factor = (float)(tree->elements) / (float)(tree->buckets);
  if (load_factor > 0.7f)
    LPT(tree_rehash)(tree, tree->buckets * 2);
  size_t hash = (LPT(hash)(code)) % tree->buckets;
  while ((tree->slots[hash].code & LPT_PRESENT_BIT) != 0)
  {
    if ((tree->slots[hash].code & LPT_BITS_MASK)
         == (code.code & LPT_BITS_MASK)) {
      debug("exit tree_insert: already present\n");
      return false; // already present
    }
    hash = (hash + 1) % tree->buckets;
  }
  tree->slots[hash].code = code.code 
    | (leaf ? LPT_LEAF_BIT : 0) 
    | (mark ? LPT_MARK_BIT : 0) 
    | LPT_PRESENT_BIT;
  tree->elements++;
  debug("exit tree_insert: inserted at slot %zu\n", hash);
  return true;
}

int LPT(tree_find)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_find: code %016lx\n", code.code);
  size_t hash = (LPT(hash)(code)) % tree->buckets;
  while ((tree->slots[hash].code & LPT_PRESENT_BIT) != 0)
  {
    if ((tree->slots[hash].code & LPT_BITS_MASK) 
        == (code.code & LPT_BITS_MASK)) {
      debug("exit tree_find: found at slot %zu\n", hash);
      return (int)hash;
    }
    hash = (hash + 1) % tree->buckets;
  }
  debug("exit tree_find: not found\n");
  return -1; // not found
}

void LPT(tree_rehash)(LPT(tree) *tree, size_t new_buckets)
{
  debug("enter tree_rehash: %zu buckets\n", new_buckets);
  lpt *old_slots = tree->slots;
  size_t old_buckets = tree->buckets;
  tree->slots = (lpt *)calloc(new_buckets, sizeof(lpt));
  tree->buckets = new_buckets;
  tree->elements = 0;
  for (size_t i = 0; i < old_buckets; ++i)
  {
    if ((old_slots[i].code & LPT_PRESENT_BIT) != 0)
    {
      bool leaf = (old_slots[i].code & LPT_LEAF_BIT) != 0;
      bool mark = (old_slots[i].code & LPT_MARK_BIT) != 0;
      lpt code = {old_slots[i].code & LPT_BITS_MASK};
      LPT(tree_insert)(tree, code, leaf, mark);
    }
  }
  free(old_slots);
  debug("exit tree_rehash\n");
}

void LPT(tree_mark)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_mark: code %016lx\n", code.code);
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    tree->slots[index].code |= LPT_MARK_BIT;
    debug("exit tree_mark\n");
  } else {
    debug("exit tree_mark: code not found\n");
  }
}

void LPT(tree_unmark)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_unmark: code %016lx\n", code.code);
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    tree->slots[index].code &= ~LPT_MARK_BIT;
    debug("exit tree_unmark\n");
  } else {
    debug("exit tree_unmark: code not found\n");
  } 
}

void LPT(tree_unmark_all)(LPT(tree) *tree) 
{
  for(size_t i=0;i<tree->buckets;++i) {
    if((tree->slots[i].code & LPT_PRESENT_BIT) != 0)
      tree->slots[i].code &= ~LPT_MARK_BIT;
  }
}

bool LPT(tree_is_marked)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_is_marked: code %016lx\n", code.code);
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    debug("exit tree_is_marked: found\n");
    return (tree->slots[index].code & LPT_MARK_BIT) != 0;
  }
  debug("exit tree_is_marked: not found\n");
  return false;
}

void LPT(tree_leaf)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_leaf: code %016lx\n", code.code);
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) { 
    tree->slots[index].code |= LPT_LEAF_BIT;
    debug("exit tree_leaf\n");
  } else {
    debug("exit tree_leaf: code not found\n");
  }
}

void LPT(tree_unleaf)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_unleaf: code %016lx\n", code.code);
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    tree->slots[index].code &= ~LPT_LEAF_BIT;
    debug("exit tree_unleaf\n");
  } else {
    debug("exit tree_unleaf: code not found\n");
  } 
}

bool LPT(tree_is_leaf)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_is_leaf: code %016lx\n", code.code);
  int index = LPT(tree_find)(tree, code);
  if (index >= 0) {
    debug("exit tree_is_leaf: found\n");
    return (tree->slots[index].code & LPT_LEAF_BIT) != 0;
  }   
  debug("exit tree_is_leaf: not found\n");
  return false;
}

bool LPT(tree_exists)(LPT(tree) *tree, lpt code)
{
  debug("enter tree_exists: code %016lx\n", code.code);
  int index = LPT(tree_find)(tree, code);
  debug("exit tree_exists: %s\n", index >= 0 ? "true" : "false");
  return index >= 0;
}

void LPT(tree_simple_bisect)(LPT(tree) *tree, lpt code)
{
  //assert(LPT(tree_exists)(tree, code) && LPT(tree_is_leaf)(tree, code))
  LPT(tree_unleaf)(tree, code);
  lpt c0 = LPT(child)(code, 0);
  lpt c1 = LPT(child)(code, 1);
  LPT(tree_insert)(tree, c0, true, false);
  LPT(tree_insert)(tree, c1, true, false);
}

void LPT(tree_compat_bisect)(LPT(tree) *tree, lpt code, void (*subdivided)(lpt,void*), void *udata)
{
  LPT(tree_mark)(tree, code);
  for(int i=0;i<=DIM;++i) {
    if(i==DIM || i==LPT(level)(code)) continue;
    lpt n;
    if(LPT(neighbor)(code, i, &n)) {
      if(!LPT(tree_exists)(tree, n)) {
        lpt p = LPT(parent)(n);
        LPT(tree_compat_bisect)(tree, p, subdivided, udata);
      }
      if(LPT(tree_is_leaf)(tree, n) && !LPT(tree_is_marked)(tree, n)) {
        LPT(tree_compat_bisect)(tree, n, subdivided, udata);
      }
    }
  }
  LPT(tree_simple_bisect)(tree, code);
  LPT(tree_unmark)(tree, code);
  if(subdivided) subdivided(code, udata);
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
    lpt rr;
    if(LPT(tree_neighbor)(tree, n, j, &rr)) {
      if((rr.code & LPT_BITS_MASK) == (r.code & LPT_BITS_MASK))
        return j;
    }
  }
  return -1;
}

lpt LPT(find_root)(double * p, double * w) {
  debug("enter find_root\n");
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
  lpt r;
  LPT(init)(&r, code);
  w[0]=(1.0-p[a[1-1]])/2.0;
  for(i=1;i<DIM;++i) w[i]=(p[a[i-1]]-p[a[(i+1)-1]])/2.0;
  w[DIM]=(p[a[DIM-1]]+1.0)/2.0;
  debug("exit find_root\n");
  return r;
}

lpt LPT(tree_search_rec)(LPT(tree) *tree, double * p, lpt r, double * w) {
  
  debug("enter tree_search_rec: code %016lx\n", r.code);
  if(LPT(tree_is_leaf)(tree, r)) {
    debug("exit tree_search_rec: leaf found\n");
    return r;
  }
  int l=LPT(level)(r);
  if(w[l]<=w[DIM]) {
    double t=w[l];
    w[l]=2.0*w[l];
    w[DIM]=w[DIM]-t;
    debug("going to child 0\n");
    return LPT(tree_search_rec)(tree, p, LPT(child)(r, 0), w);
  } else {
    double t=w[DIM];
    w[l]=w[l]-t;
    for(int i=DIM;i>l;--i) w[i]=w[i-1];
    w[l]=2.0*t;
    debug("going to child 1\n");
    return LPT(tree_search_rec)(tree, p, LPT(child)(r, 1), w);
  }
}

lpt LPT(tree_search_w)(LPT(tree) *tree, double * p, double * w) {
  debug("enter tree_search_w\n");
  lpt r=LPT(find_root)(p,w);
  lpt result = LPT(tree_search_rec)(tree, p, r, w);
  debug("exit tree_search_w\n");
  return result;
}

lpt LPT(tree_search)(LPT(tree) *tree, double * p) {
  debug("enter tree_search\n");
  double w[DIM+1];
  lpt r=LPT(tree_search_w)(tree, p, w);
  debug("exit tree_search\n");
  return r; 
}

#define delete_bit(byte, i) (((byte>>1)&((~0u)<<i))|((~((~0u)<<i))&byte))
#define insert_bit(byte, i) (((((~0u)<<i)&byte)<<1)|((~((~0u)<<i))&byte))

void LPT(tree_search_all_rec)(LPT(tree) *tree, lpt r, unsigned int faces, void (*visit)(lpt,void*),void *udata) {
  debug("enter tree_search_all_rec: code %016lx\n", r.code);
  LPT(tree_mark)(tree, r);
  for(unsigned int i=0;i<=DIM;++i) {
    if((faces & (1<<i))!=0) continue;
    lpt n;
    if(LPT(tree_neighbor)(tree, r, i, &n)) {
      if(!LPT(tree_is_marked)(tree, n)) {
        int k = LPT(tree_neighbor_index)(tree, r, n);
        unsigned int new_faces =
          insert_bit(delete_bit(faces,i), k);
        LPT(tree_search_all_rec)(tree, n, new_faces, visit, udata);
      }
    }
  }
  debug("visiting code %016lx\n", r.code);
  visit(r, udata);
  //LPT(tree_unmark)(tree, r);
  debug("exit tree_search_all_rec: code %016lx\n", r.code);
}

void LPT(tree_search_all)(LPT(tree) *tree, double * p, void (*visit)(lpt,void*), void *udata) {
  debug("enter tree_search_all\n");
  double w[DIM+1];
  lpt r=LPT(tree_search_w)(tree, p, w);
  unsigned int faces=0;
  for(int i=0;i<=DIM;++i) if(w[i]!=0.0) faces+= (1<<i);
  LPT(tree_unmark_all)(tree);
  LPT(tree_search_all_rec)(tree, r, faces, visit, udata);
  debug("exit tree_search_all\n");
}

void LPT(tree_visit_leafs)(LPT(tree) *tree, void (*visit)(lpt,void*), void *udata)
{
  debug("enter tree_visit_leafs\n");
  for(size_t i=0;i<tree->buckets;++i) {
    if(tree->slots[i].code & (LPT_LEAF_BIT)) {
      lpt code = {tree->slots[i].code & LPT_BITS_MASK};
      debug("Visiting slot: %zu\t", i);
      visit(code, udata);
    }
  }
  debug("exit tree_visit_leafs\n");
}

