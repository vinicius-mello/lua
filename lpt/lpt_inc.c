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
      LPT(sigperm_set)(r, LPT(lft)(code, lminus));
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

