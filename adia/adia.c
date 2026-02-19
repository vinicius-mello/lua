#include <math.h>
#include <stdlib.h>
#include "lua.h"
#include "lauxlib.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "fi_lib.h"


enum rpn_ops {
  rpn_add,
  rpn_sub,
  rpn_unm,
  rpn_mul,
  rpn_div,
  rpn_pow,
  rpn_sin,
  rpn_cos,
  rpn_tan,
  rpn_exp,
  rpn_log,
};

enum rpn_type {
  rpn_var,
  rpn_op,
  rpn_num
};

struct rpn_node {
  union {
    double value;
    enum rpn_ops op;
    int var;
  };
  enum rpn_type type;
};

struct rpn_cmds {
  struct rpn_node *nodes;
  size_t count;
};

struct rpn_cmds * rpn_new(lua_State *L, size_t count) {
  struct rpn_cmds *cmds = (struct rpn_cmds *)lua_newuserdata(L, sizeof(struct rpn_cmds)+sizeof(struct rpn_node)*count);
  luaL_setmetatable(L,"RPN");
  cmds->nodes = (struct rpn_node *)(cmds+1);
  cmds->count = count;
  return cmds;
}

static int Fvar(lua_State * L) {
  struct rpn_cmds *cmds = rpn_new(L, 1);
  cmds->nodes[0].type = rpn_var;
  cmds->nodes[0].var = luaL_checkinteger(L, 1);
  return 1;
} 

#define RPN_OP(name) \
static int F##name(lua_State * L) { \
  struct rpn_cmds *a; \
  if(lua_isnumber(L,1)) {\
    a = rpn_new(L, 1); \
    a->nodes[0].type = rpn_num; \
    a->nodes[0].value = lua_tonumber(L,1); \
  } else {\
    a = (struct rpn_cmds *)luaL_checkudata(L, 1, "RPN");\
  }\
  struct rpn_cmds *b; \
  if(lua_isnumber(L,2)) {\
    b = rpn_new(L, 1); \
    b->nodes[0].type = rpn_num; \
    b->nodes[0].value = lua_tonumber(L,2); \
  } else {\
    b = (struct rpn_cmds *)luaL_checkudata(L, 2, "RPN");\
  }\
  struct rpn_cmds *cmds = rpn_new(L, a->count + b->count + 1); \
  memcpy(cmds->nodes, a->nodes, sizeof(struct rpn_node)*a->count); \
  memcpy(cmds->nodes+a->count, b->nodes, sizeof(struct rpn_node)*b->count); \
  cmds->nodes[a->count+b->count].type = rpn_op; \
  cmds->nodes[a->count+b->count].op = rpn_##name; \
  return 1; \
}

RPN_OP(add)
RPN_OP(sub)
RPN_OP(mul)
RPN_OP(div)
RPN_OP(pow)

#define RPN_FUNC(name) \
static int F##name(lua_State * L) { \
  struct rpn_cmds *a = (struct rpn_cmds *)luaL_checkudata(L, 1, "RPN");\
  struct rpn_cmds *cmds = rpn_new(L, a->count + 1); \
  memcpy(cmds->nodes, a->nodes, sizeof(struct rpn_node)*a->count); \
  cmds->nodes[a->count].type = rpn_op; \
  cmds->nodes[a->count].op = rpn_##name; \
  return 1; \
}

RPN_FUNC(sin)
RPN_FUNC(cos)
RPN_FUNC(tan)
RPN_FUNC(exp)
RPN_FUNC(log)
RPN_FUNC(unm) 

static int Fprint(lua_State * L) {
  struct rpn_cmds *cmds = (struct rpn_cmds *)luaL_checkudata(L, 1, "RPN");
  for(size_t i=0;i<cmds->count;++i) {
    if(cmds->nodes[i].type == rpn_op) {
      switch(cmds->nodes[i].op) {
        case rpn_add: printf("+ "); break;
        case rpn_sub: printf("- "); break;
        case rpn_mul: printf("* "); break;
        case rpn_div: printf("/ "); break;
        case rpn_pow: printf("^ "); break;
        case rpn_sin: printf("sin "); break;
        case rpn_cos: printf("cos "); break;
        case rpn_tan: printf("tan "); break; 
        case rpn_exp: printf("exp "); break;
        case rpn_log: printf("log "); break;
        case rpn_unm: printf("unm "); break;
      }
    } else if(cmds->nodes[i].type == rpn_var) {
      printf("x%d ", cmds->nodes[i].var);
    } else {
      printf("%g ", cmds->nodes[i].value);
    }
  }
  printf("\n");
  return 0;
}

static int Feval_n(lua_State * L) {
  struct rpn_cmds *cmds = (struct rpn_cmds *)luaL_checkudata(L, 1, "RPN");
  double stack[256];
  size_t sp = 0;
  for(size_t i=0;i<cmds->count;++i) {
    if(cmds->nodes[i].type == rpn_op) {
      switch(cmds->nodes[i].op) {
        case rpn_add: stack[sp-2] = stack[sp-2] + stack[sp-1]; sp--; break;
        case rpn_sub: stack[sp-2] = stack[sp-2] - stack[sp-1]; sp--; break;
        case rpn_mul: stack[sp-2] = stack[sp-2] * stack[sp-1]; sp--; break;
        case rpn_div: stack[sp-2] = stack[sp-2] / stack[sp-1]; sp--; break;
        case rpn_pow: stack[sp-2] = pow(stack[sp-2], stack[sp-1]); sp--; break;
        case rpn_sin: stack[sp-1] = sin(stack[sp-1]); break;
        case rpn_cos: stack[sp-1] = cos(stack[sp-1]); break;
        case rpn_tan: stack[sp-1] = tan(stack[sp-1]); break; 
        case rpn_exp: stack[sp-1] = exp(stack[sp-1]); break;
        case rpn_log: stack[sp-1] = log(stack[sp-1]); break;
        case rpn_unm: stack[sp-1] = -stack[sp-1]; break;
      }
    } else if(cmds->nodes[i].type == rpn_var) {
      stack[sp++] = luaL_checknumber(L, cmds->nodes[i].var + 1);
    } 
    else {
      stack[sp++] = cmds->nodes[i].value;
    }
  }
  lua_pushnumber(L, sp > 0 ? stack[0] : 0);
  return 1;
}

static double q_ipow(double x, int n)
{
  double r;
  for (r=1.0; n>0; n>>=1)
  {
    if (n&1) r*=x;
    x*=x;
  }
  return r;
}

static interval j_ipow(interval i, int n)			/** pow(x,n) */
{
  interval r;
  if (n<0) {
    r.INF=0.0;
    r.SUP=0.0;
    return r;
  } else if (n==0) {
    r.INF=1.0;
    r.SUP=1.0;
    return r;
  } else if (n==1) {
    return i; 
  } else if (n==2) {  
    r=j_sqr(i);
    return r;
  }
  double p=i.INF;
  double q=i.SUP;
  double P,Q;
  P=q_ipow(p,n);
  Q=q_ipow(q,n);
  if (n%2==1 || p>=0.0) { //Pout(r,P,Q);
    r.INF = q_pred(P);
    r.SUP = q_succ(Q);
  } else if (q<0.0) { //Pout(r,Q,P);
    r.INF = q_pred(Q);
    r.SUP = q_succ(P);
  } else { //   Pset(r,0.0,q_max(P,Q));
    P=q_succ(P);
    Q=q_succ(Q);
    r.INF = 0.0;
    r.SUP = q_max(P,Q);
  }
  return r;
}

interval * interval_new(lua_State *L) {
  interval *i = (interval *)lua_newuserdata(L, sizeof(interval)); 
  luaL_setmetatable(L,"FI");
  return i;
}

static int Fnew_i(lua_State * L) {
  interval * i = interval_new(L);
  i->INF = luaL_checknumber(L, 1);
  i->SUP = luaL_checknumber(L, 2);
  return 1;
}

static int Iprint(lua_State * L) {
  interval * i = (interval *)luaL_checkudata(L, 1, "FI");
  printf("[%g, %g]\n", i->INF, i->SUP);
  return 0;
}

#define FI_OP(name) \
static int I##name(lua_State * L) { \
  if(lua_isnumber(L,1)) { \
    double a = lua_tonumber(L,1); \
    interval * b = (interval *)luaL_checkudata(L, 2, "FI"); \
    interval * c = interval_new(L); \
    *c = name##_di(a, *b); \
    return 1;  \
  } \
  if(lua_isnumber(L,2)) { \
    interval * a = (interval *)luaL_checkudata(L, 1, "FI"); \
    double b = lua_tonumber(L,2); \
    interval * c = interval_new(L); \
    *c = name##_id(*a, b); \
    return 1;  \
  } \
  interval * a = (interval *)luaL_checkudata(L, 1, "FI"); \
  interval * b = (interval *)luaL_checkudata(L, 2, "FI"); \
  interval * c = interval_new(L); \
  *c = name##_ii(*a, *b); \
  return 1; \
}

FI_OP(add)
FI_OP(sub)
FI_OP(mul)
FI_OP(div)

static int Ipow(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  int b = lua_tointeger(L,2);
  interval * c = interval_new(L);
  *c = j_ipow(*a, b);
  return 1;
}

#define FI_FUNC(name) \
static int I##name(lua_State * L) { \
  interval * a = (interval *)luaL_checkudata(L, 1, "FI"); \
  interval * c = interval_new(L); \
  *c = j_##name(*a); \
  return 1; \
}

FI_FUNC(sin)
FI_FUNC(cos)
FI_FUNC(tan)
FI_FUNC(exp)
FI_FUNC(log)

interval j_unm(interval i) {
  interval r;
  r.INF = -i.SUP;
  r.SUP = -i.INF;
  return r;
}

static int Iunm(lua_State * L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  interval * c = interval_new(L);
  *c = j_unm(*a);
  return 1; 
}

static int Itostring(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  lua_pushfstring(L, "[%g, %g]", a->INF, a->SUP);
  return 1;
}

static int Iextremes(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  lua_pushnumber(L, a->INF);
  lua_pushnumber(L, a->SUP);
  return 2;
}

static int Idiam(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  lua_pushnumber(L, q_diam(*a));
  return 1;
}

static int Iinf(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  lua_pushnumber(L, a->INF);
  return 1;
}

static int Isup(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  lua_pushnumber(L, a->SUP);
  return 1;
}

static int Imid(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  lua_pushnumber(L, q_mid(*a));
  return 1;
}

static int Icontains(lua_State *L) {
  interval * a = (interval *)luaL_checkudata(L, 1, "FI");
  double t = luaL_checknumber(L, 2);
  lua_pushboolean(L, in_di(t, *a));
  return 1;
}

static int Feval_i(lua_State * L) {
  struct rpn_cmds *cmds = (struct rpn_cmds *)luaL_checkudata(L, 1, "RPN");
  interval stack[256];
  size_t sp = 0;
  for(size_t i=0;i<cmds->count;++i) {
    if(cmds->nodes[i].type == rpn_op) {
      switch(cmds->nodes[i].op) {
        case rpn_add: stack[sp-2] = add_ii(stack[sp-2], stack[sp-1]); sp--; break;
        case rpn_sub: stack[sp-2] = sub_ii(stack[sp-2], stack[sp-1]); sp--; break;
        case rpn_mul: stack[sp-2] = mul_ii(stack[sp-2], stack[sp-1]); sp--; break;
        case rpn_div: stack[sp-2] = div_ii(stack[sp-2], stack[sp-1]); sp--; break;
        case rpn_pow: stack[sp-2] = j_ipow(stack[sp-2], (int)stack[sp-1].INF); sp--; break;
        case rpn_sin: stack[sp-1] = j_sin(stack[sp-1]); break;
        case rpn_cos: stack[sp-1] = j_cos(stack[sp-1]); break;
        case rpn_tan: stack[sp-1] = j_tan(stack[sp-1]); break; 
        case rpn_exp: stack[sp-1] = j_exp(stack[sp-1]); break;
        case rpn_log: stack[sp-1] = j_log(stack[sp-1]); break;
        case rpn_unm: stack[sp-1] = j_unm(stack[sp-1]); break;
      }
    } else if(cmds->nodes[i].type == rpn_var) {
      interval * var = (interval *)luaL_checkudata(L, cmds->nodes[i].var + 1, "FI");
      stack[sp++] = *var;
    } 
    else {
      interval num;
      num.INF = cmds->nodes[i].value;
      num.SUP = num.INF;
      stack[sp++] = num;
    }
  }
  interval *c = interval_new(L);
  *c = stack[0];
  return 1;
}

void dual_number_add(int nvars, double *a, double *b, double *c) {
  for(int i=0;i<=nvars;++i) {
    c[i] = a[i] + b[i];
  }
}

void dual_number_sub(int nvars, double *a, double *b, double *c) {
  for(int i=0;i<=nvars;++i) {
    c[i] = a[i] - b[i];
  }
}

void dual_number_mul(int nvars, double *a, double *b, double *c) {
  c[0] = a[0] * b[0];
  for(int i=1;i<=nvars;++i) {
    c[i] = a[i] * b[0] + a[0] * b[i];
  }
}

void dual_number_div(int nvars, double *a, double *b, double *c) {
  c[0] = a[0] / b[0];
  double t = b[0] * b[0];
  for(int i=1;i<=nvars;++i) {
    c[i] = (a[i] * b[0] - a[0] * b[i]) / t;
  }
}

void dual_number_ipow(int nvars, double *a, int b, double *c) {
  double t = q_ipow(a[0], b-1);
  c[0] = a[0]*t;
  t = t * b;
  for(int i=1;i<=nvars;++i) {
    c[i] = t * a[i];
  }
}

void dual_number_unm(int nvars, double *a, double *c) {
  for(int i=0;i<=nvars;++i) {
    c[i] = -a[i];
  }
}
  
void dual_number_sin(int nvars, double *a, double *c) {
  c[0] = sin(a[0]);
  double t = cos(a[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = t * a[i];
  }
}

void dual_number_cos(int nvars, double *a, double *c) {
  c[0] = cos(a[0]);
  double t = -sin(a[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = t * a[i];
  }
}

void dual_number_tan(int nvars, double *a, double *c) {
  c[0] = tan(a[0]);
  double t = 1 + c[0] * c[0];
  for(int i=1;i<=nvars;++i) {
    c[i] = t * a[i];
  }
}   

void dual_number_exp(int nvars, double *a, double *c) {
  c[0] = exp(a[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = c[0] * a[i];
  }
}

void dual_number_log(int nvars, double *a, double *c) {
  c[0] = log(a[0]);
  double t = 1/a[0];
  for(int i=1;i<=nvars;++i) {
    c[i] = t * a[i];
  }
}

static int Feval_d(lua_State * L) {
  struct rpn_cmds *cmds = (struct rpn_cmds *)luaL_checkudata(L, 1, "RPN");
  const int stack_size = 32;
  double * stack[stack_size];
  int nvars = lua_gettop(L) - 1;
  double * temp = (double *)malloc(sizeof(double)*(nvars+1)*(stack_size+1));
  for(int i=0;i<stack_size;++i) {
    stack[i] = temp + (i+1)*(nvars+1);
  }
  size_t sp = 0;
  for(size_t i=0;i<cmds->count;++i) {
    if(cmds->nodes[i].type == rpn_op) {
      switch(cmds->nodes[i].op) {
        case rpn_add: dual_number_add(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_sub: dual_number_sub(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_mul: dual_number_mul(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_div: dual_number_div(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_pow: dual_number_ipow(nvars, stack[sp-2], (int)stack[sp-1][0], temp); sp--; break;
        case rpn_sin: dual_number_sin(nvars, stack[sp-1], temp); break;
        case rpn_cos: dual_number_cos(nvars, stack[sp-1], temp); break;
        case rpn_tan: dual_number_tan(nvars, stack[sp-1], temp); break; 
        case rpn_exp: dual_number_exp(nvars, stack[sp-1], temp); break;
        case rpn_log: dual_number_log(nvars, stack[sp-1], temp); break;
        case rpn_unm: dual_number_unm(nvars, stack[sp-1], temp); break;
      }
      memcpy(stack[sp-1], temp, sizeof(double)*(nvars+1));
    } else if(cmds->nodes[i].type == rpn_var) {
      int var_index = cmds->nodes[i].var;
      double * num = stack[sp++];
      num[0] = luaL_checknumber(L, var_index+1);
      for(int j=1;j<=nvars;++j) {
        num[j] = (j == var_index) ? 1 : 0;
      }
    }
    else {
      double * num = stack[sp++];
      num[0] = cmds->nodes[i].value;
      for(int j=1;j<=nvars;++j) {
        num[j] = 0;
      }
    }
  }
  lua_pushnumber(L, stack[0][0]);
  lua_newtable(L);
  for(int i=1;i<=nvars;++i) {
    lua_pushnumber(L, stack[0][i]);
    lua_rawseti(L, -2, i);
  } 
  free(temp);
  return 2;
}

void dual_interval_add(int nvars, interval *a, interval *b, interval *c) {
  for(int i=0;i<=nvars;++i) {
    c[i] = add_ii(a[i], b[i]);
  }
}

void dual_interval_sub(int nvars, interval *a, interval *b, interval *c) {
  for(int i=0;i<=nvars;++i) {
    c[i] = sub_ii(a[i], b[i]);
  }
}

void dual_interval_mul(int nvars, interval *a, interval *b, interval *c) {
  c[0] = mul_ii(a[0], b[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = add_ii(mul_ii(a[i], b[0]), mul_ii(a[0], b[i]));
  }
}

void dual_interval_div(int nvars, interval *a, interval *b, interval *c) {
  c[0] = div_ii(a[0], b[0]);
  interval b0_sqr = j_sqr(b[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = div_ii(sub_ii(mul_ii(a[i], b[0]), mul_ii(a[0], b[i])), b0_sqr);
  }
}

void dual_interval_ipow(int nvars, interval *a, int b, interval *c) {
  interval t = j_ipow(a[0], b-1);
  c[0] = mul_ii(t, a[0]);
  t = mul_id(t, (double)b);
  for(int i=1;i<=nvars;++i) {
    c[i] = mul_ii(t, a[i]);
  }
}

void dual_interval_unm(int nvars, interval *a, interval *c) {
  for(int i=0;i<=nvars;++i) {
    c[i] = j_unm(a[i]);
  }
}

void dual_interval_sin(int nvars, interval *a, interval *c) {
  c[0] = j_sin(a[0]);
  interval t = j_cos(a[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = mul_ii(t, a[i]);
  }
}

void dual_interval_cos(int nvars, interval *a, interval *c) {
  c[0] = j_cos(a[0]);
  interval t = j_unm(j_sin(a[0]));
  for(int i=1;i<=nvars;++i) {
    c[i] = mul_ii(t, a[i]);
  }
}

void dual_interval_tan(int nvars, interval *a, interval *c) {
  c[0] = j_tan(a[0]);
  interval t = add_id(j_sqr(c[0]), 1.0);
  for(int i=1;i<=nvars;++i) {
    c[i] = mul_ii(t, a[i]);
  }
}

void dual_interval_exp(int nvars, interval *a, interval *c) {
  c[0] = j_exp(a[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = mul_ii(c[0], a[i]);
  }
}

void dual_interval_log(int nvars, interval *a, interval *c) {
  c[0] = j_log(a[0]);
  for(int i=1;i<=nvars;++i) {
    c[i] = div_ii(a[i], a[0]);
  }
}

static int Feval_di(lua_State * L) {
  struct rpn_cmds *cmds = (struct rpn_cmds *)luaL_checkudata(L, 1, "RPN");
  const int stack_size = 32;
  interval * stack[stack_size];
  int nvars = lua_gettop(L) - 1;
  interval * temp = (interval *)malloc(sizeof(interval)*(nvars+1)*(stack_size+1));
  for(int i=0;i<stack_size;++i) {
    stack[i] = temp + (i+1)*(nvars+1);
  }
  size_t sp = 0;
  for(size_t i=0;i<cmds->count;++i) {
    if(cmds->nodes[i].type == rpn_op) {
      switch(cmds->nodes[i].op) {
        case rpn_add: dual_interval_add(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_sub: dual_interval_sub(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_mul: dual_interval_mul(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_div: dual_interval_div(nvars, stack[sp-2], stack[sp-1], temp); sp--; break;
        case rpn_pow: dual_interval_ipow(nvars, stack[sp-2], (int)stack[sp-1][0].INF, temp); sp--; break;
        case rpn_sin: dual_interval_sin(nvars, stack[sp-1], temp); break;
        case rpn_cos: dual_interval_cos(nvars, stack[sp-1], temp); break;
        case rpn_tan: dual_interval_tan(nvars, stack[sp-1], temp); break; 
        case rpn_exp: dual_interval_exp(nvars, stack[sp-1], temp); break;
        case rpn_log: dual_interval_log(nvars, stack[sp-1], temp); break;
        case rpn_unm: dual_interval_unm(nvars, stack[sp-1], temp); break;
      }
      memcpy(stack[sp-1], temp, sizeof(interval)*(nvars+1));
    } else if(cmds->nodes[i].type == rpn_var) {
      int var_index = cmds->nodes[i].var;
      interval * num = stack[sp++];
      num[0] = *(interval *)luaL_checkudata(L, var_index+1, "FI");
      for(int j=1;j<=nvars;++j) {
        double k = (j == var_index) ? 1 : 0;
        num[j] = (interval){k, k};
      }
    }
    else {
      interval * num = stack[sp++];
      double k = cmds->nodes[i].value;
      num[0] = (interval){k, k};
      for(int j=1;j<=nvars;++j) {
        num[j] = (interval){0, 0};
      }
    }
  }
  interval *c = interval_new(L);
  *c = stack[0][0];
  lua_newtable(L);
  for(int i=1;i<=nvars;++i) {
    interval *derivative = interval_new(L);
    *derivative = stack[0][i];
    lua_rawseti(L, -2, i);
  } 
  free(temp);
  return 2;
}


static const luaL_Reg FI[] =
{
  { "new",	Fnew_i},
  { "__add", Iadd},
  { "__sub", Isub},
  { "__mul", Imul},
  { "__div", Idiv},
  { "print", Iprint},
  { "__pow", Ipow},
  { "__unm", Iunm},
  { "sin", Isin},
  { "cos", Icos},
  { "tan", Itan},
  { "exp", Iexp},
  { "log", Ilog},
  { "__tostring", Itostring},
  { "extremes", Iextremes},
  { "diam", Idiam},
  { "inf", Iinf},
  { "sup", Isup},
  { "mid", Imid},
  { "contains", Icontains},
 	{ NULL,		NULL	}
};

static const luaL_Reg R[] =
{
	{ "var",	Fvar},
  { "__add", Fadd},
  { "__sub", Fsub},
  { "__mul", Fmul},
  { "__div", Fdiv},
  { "__pow", Fpow},
  { "__unm", Funm},
  { "sin", Fsin},
  { "cos", Fcos},
  { "tan", Ftan},
  { "exp", Fexp},
  { "log", Flog},
  { "print", Fprint},
  { "eval_n", Feval_n},
  { "eval_d", Feval_d},
  { "eval_i", Feval_i},
  { "eval_di", Feval_di},
  { "__call", Feval_n},
	{ NULL,		NULL	}
};


LUALIB_API int luaopen_adia(lua_State *L)
{
  luaL_newmetatable(L,"FI");
  luaL_setfuncs(L,FI,0);
  lua_pushliteral(L,"__index");
 	lua_pushvalue(L,-2);
  lua_settable(L,-3);

  luaL_newmetatable(L,"RPN");
  luaL_setfuncs(L,R,0);
 	lua_pushliteral(L,"__index");
 	lua_pushvalue(L,-2);
  lua_settable(L,-3);
	lua_pushliteral(L, "interval");
  lua_pushvalue(L, -3);
  lua_settable(L, -3);
  return 1;
}

