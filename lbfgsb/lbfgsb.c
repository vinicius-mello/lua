#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define LIBNAME  "lbfgsb"
#define LIBTYPE "lbfgsb"
#define LIBVERSION "1"
#include "lua.h"
#include "lauxlib.h"
#define lua_swap(L) lua_insert(L, -2)
#include "../array/array.h"



void setulb_ (int *n, int *m, double *x,
  double *l, double *u, int *nbd, 
  double *f, double *g, 
  double *factr, double *pgtol, 
  double *wa, int *iwa, 
  char *task, int *iprint, 
  char *csave, int_least32_t *lsave, int *isave, double *dsave, 
  size_t task_len, size_t csave_len);  

typedef struct lbfgsb_data_ {
  int n;
  int m;
  array * x;
  array * lower;
  array * upper;
  array * nbd;
  double f;
  array * g;
  double factr;
  double pgtol;
  double * wa;
  int * iwa;
  char task[60];
  int iprint;
  char csave[60];
  int_least32_t lsave[4];
  int isave[44];
  double dsave[29];
} lbfgsb_data;

typedef unsigned int uint;

static int Fnew(lua_State *L) {
  if(lua_istable(L,1)) {
    lua_getfield(L,1,"x");
    array * x_array = luaL_checkudata(L,-1,"array");
    lua_pop(L,1);
    lua_getfield(L,1,"g");
    array * g_array = luaL_checkudata(L,-1,"array");
    lua_pop(L,1);
    lua_getfield(L,1,"lower");
    array * lower_array = luaL_checkudata(L,-1,"array");
    lua_pop(L,1);
    lua_getfield(L,1,"upper");
    array * upper_array = luaL_checkudata(L,-1,"array");
    lua_pop(L,1);
    lua_getfield(L,1,"nbd");
    array * nbd_array = luaL_checkudata(L,-1,"array");
    lua_pop(L,1);
    lua_getfield(L,1,"m");
    int m = luaL_optinteger(L,-1,5);
    lua_pop(L,1);
    int n = x_array->rows*x_array->cols;
    size_t wa_len = (2*m + 5)*n + 11*m*m + 8*m;
    size_t lbfgsb_data_size = sizeof(lbfgsb_data)+sizeof(double)*wa_len + sizeof(int)*3*n;
    lbfgsb_data * data = 
      (lbfgsb_data *)lua_newuserdata(L, lbfgsb_data_size);
    memset(data, 0, lbfgsb_data_size);
    data->wa = (double *)(data + 1);
    data->iwa = (int *)(data->wa + wa_len);
    luaL_setmetatable(L,"lbfgsb");
    data->n = n;
    data->m = m;
    data->x = x_array;
    data->g = g_array;
    data->lower = lower_array;
    data->upper = upper_array;
    data->nbd = nbd_array;
    lua_getfield(L,1,"eps");
    data->pgtol = luaL_optnumber(L,-1,1e-5);
    lua_pop(L,1);
    lua_getfield(L,1,"iprint");
    data->iprint = luaL_optinteger(L,-1,1);
    lua_pop(L,1);
    lua_getfield(L,1,"factr");
    data->factr = luaL_optnumber(L,-1,1e+7);
    lua_pop(L,1);
    lua_getfield(L,1,"pgtol");
    data->pgtol = luaL_optnumber(L,-1,1e-5);
    lua_pop(L,1);
    data->task[0] = 'S';
    data->task[1] = 'T';
    data->task[2] = 'A';
    data->task[3] = 'R';
    data->task[4] = 'T';
    for(size_t i=5;i<60;++i) {
      data->task[i] = ' ';
    }
  } else {
    return luaL_error(L, "lbfgsb.new: expected table with x and g fields");
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
  lbfgsb_data * data = (lbfgsb_data *)luaL_checkudata(L,1,"lbfgsb");
  clone_array(L, data->x);
  clone_array(L, data->g);
  for(size_t icall=0;icall<100000;++icall) {
    setulb_(&data->n, &data->m, (double*)data->x->ptr,
      (double*)data->lower->ptr, (double*)data->upper->ptr, (int*)data->nbd->ptr,
      &data->f, (double*)data->g->ptr, 
      &data->factr, &data->pgtol, 
      data->wa, data->iwa, 
      data->task, &data->iprint, 
      data->csave, data->lsave, data->isave, data->dsave,
      sizeof(data->task), sizeof(data->csave));
    if(strncmp(data->task,"NEW_X",5) == 0) {
    } else if(strncmp(data->task,"FG",2) == 0) {
      lua_pushvalue(L,-3);
      lua_pushvalue(L,-3);
      lua_pushvalue(L,-3);
      lua_call(L,2,1);
      data->f = lua_tonumber(L,-1);
      lua_pop(L,1);
    } else if(strncmp(data->task,"CONV",4) == 0 || strncmp(data->task,"ABNO",4) == 0 || strncmp(data->task,"ERROR",5) == 0) {
      break;
    }
  }
  data->task[0] = 'S';
  data->task[1] = 'T';
  data->task[2] = 'A';
  data->task[3] = 'R';
  data->task[4] = 'T';
  for(size_t i=5;i<60;++i) {
    data->task[i] = ' ';
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

LUALIB_API int luaopen_lbfgsb(lua_State *L)
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
