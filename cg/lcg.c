#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#define LIBNAME  "cg"
#define LIBTYPE "cg"
#define LIBVERSION "1"
#include "lua.h"
#include "lauxlib.h"
#define lua_swap(L) lua_insert(L, -2)
#include "../array/array.h"

void cgfam_ (int *n, double *x, double *f, 
  double *g, double *d, double *gold, int iprint[2], double *eps, double *w, 
  int *iflag, int *irest, int *method, int_least32_t *finish);

typedef struct cg_data_ {
  int n;
  array * x;
  double f;
  array * g;
  double * d;
  double * gold;
  int iprint[2]; 
  double eps; 
  double * w;
  int iflag; 
  int irest; 
  int method; 
  int finish;
} cg_data;

typedef unsigned int uint;

static int Fnew(lua_State *L) {
  if(lua_istable(L,1)) {
    lua_getfield(L,1,"x");
    array * x_array = luaL_checkudata(L,-1,"array");
    lua_pop(L,1);
    lua_getfield(L,1,"g");
    array * g_array = luaL_checkudata(L,-1,"array");
    lua_pop(L,1);
    int n = x_array->rows*x_array->cols;
    cg_data * data = (cg_data *)lua_newuserdata(L, 
      sizeof(cg_data)+sizeof(double)*3*n);
    luaL_setmetatable(L,"cg");
    data->d = (double *)(data + 1);
    data->gold = data->d + n;
    data->w = data->gold + n;
    data->n = n;
    data->x = x_array;
    data->g = g_array;
    lua_getfield(L,1,"eps");
    data->eps = luaL_optnumber(L,-1,1e-5);
    lua_pop(L,1);
    lua_getfield(L,1,"iprint1");
    data->iprint[0] = luaL_optinteger(L,-1,1);
    lua_pop(L,1);
    lua_getfield(L,1,"iprint2");
    data->iprint[1] = luaL_optinteger(L,-1,0);
    lua_pop(L,1);
    lua_getfield(L,1,"method");
    data->method = luaL_optinteger(L,-1,2);
    lua_pop(L,1);
    lua_getfield(L,1,"irest");
    data->irest = luaL_optinteger(L,-1,1);
    lua_pop(L,1);
    data->finish = 0;
    data->f = 0.0;
    data->iflag = 0;
  } else {
    return luaL_error(L, "cg.new: expected table with x and g fields");
  }
  return 1;
}

static int clone_array(lua_State *L, array * a) {
  array * b =(array *)lua_newuserdata(L, sizeof(array));
  b->rows = a->rows;
  b->cols = a->cols;
  b->ptr = a->ptr;
  b->sizeof_entry = a->sizeof_entry;
  b->entry_type = a->entry_type;
  luaL_setmetatable(L,"array");
  return 1;
}

static int Fminimize(lua_State *L) {
  cg_data * data = (cg_data *)luaL_checkudata(L,1,"cg");
  clone_array(L, data->x);
  clone_array(L, data->g);
  lua_pushvalue(L,-3);
  lua_pushvalue(L,-3);
  lua_pushvalue(L,-3);
  lua_call(L,2,1);
  data->f = lua_tonumber(L,-1);
  lua_pop(L,1);
  for(size_t icall=0;icall<100000;++icall) {
    cgfam_(&data->n, (double*)data->x->ptr, &data->f, (double*)data->g->ptr, data->d, data->gold,
      data->iprint, &data->eps, data->w, &data->iflag, &data->irest,
      &data->method, &data->finish);
    int iflag = data->iflag;
    if(iflag == 0) {
      break;
    } else if(iflag == 1) {
      lua_pushvalue(L,-3);
      lua_pushvalue(L,-3);
      lua_pushvalue(L,-3);
      lua_call(L,2,1);
      data->f = lua_tonumber(L,-1);
      lua_pop(L,1);
    } else if(iflag == 2) {
      double tlev = data->eps*(1.0+fabs(data->f));
      data->finish = 1;
      for(int i=0;i<data->n;++i) {
        if(fabs(((double*)data->g->ptr)[i])>tlev) {
          data->finish = 0;
          break;
        }
      }
    } else {
      return luaL_error(L, "cg.minimize: cgfam_ returned with iflag=%d", data->iflag);
    }
  } 
  lua_pushnumber(L,data->f);
  return 1;
}
              
static const luaL_Reg R[] =
{
    { "new", Fnew },
    { "minimize", Fminimize },
    { NULL,     NULL    }
};

LUALIB_API int luaopen_cg(lua_State *L)
{
  luaL_newmetatable(L,LIBTYPE);
 	luaL_setfuncs(L,R,0);
  lua_pushliteral(L,"version");
  lua_pushliteral(L,LIBVERSION);
  lua_settable(L,-3);
  lua_pushliteral(L,"__index");
  lua_pushvalue(L,-2);
  lua_settable(L,-3);
  return 1;
}
