#include <stdlib.h>
#include <string.h>
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
  double * wa;
  int * iwa;
  char csave[60];
  int_least32_t lsave[4];
  int isave[44];
  double dsave[29];
  char task[60];
} lbfgsb_data;

typedef unsigned int uint;

void array_double(lua_State *L, uint n) {
  luaL_getmetatable(L,"array");
  lua_pushstring(L, "double");
  lua_gettable(L,-2);
  lua_newtable(L);
  lua_pushstring(L, "rows");
  lua_pushinteger(L, n);
  lua_settable(L,-3);
  lua_call(L,1,1);
  lua_remove(L,-2);
}

void array_int(lua_State *L, uint n) {
  luaL_getmetatable(L,"array");
  lua_pushstring(L, "int");
  lua_gettable(L,-2);
  lua_newtable(L);
  lua_pushstring(L, "rows");
  lua_pushinteger(L, n);
  lua_settable(L,-3);
  lua_call(L,1,1);
  lua_remove(L,-2);
}

static int Fnew(lua_State *L) {
  if(lua_istable(L,1)) {
    lua_getfield(L,1,"n_max");
    int n_max = luaL_checkinteger(L,-1);
    lua_pop(L,1);
    lua_getfield(L,1,"m_max");
    int m_max = luaL_checkinteger(L,-1);
    lua_pop(L,1); 
    luaL_getmetatable(L,LIBTYPE);
    lua_setmetatable(L,1);
    lua_pushstring(L, "n");
    lua_pushinteger(L, n_max);
    lua_settable(L,1);
    lua_pushstring(L, "m");
    lua_pushinteger(L, m_max);
    lua_settable(L,1);
    lua_pushstring(L, "x");
    array_double(L, n_max);
    lua_settable(L,1);
    lua_pushstring(L, "f");
    lua_pushnumber(L, 0.0);
    lua_settable(L,1);
    lua_pushstring(L, "g");
    array_double(L, n_max);
    lua_settable(L,1);
    lua_pushstring(L, "__data__");
    size_t wa_len = (2*m_max + 5)*n_max + 11*m_max*m_max + 8*m_max;
    size_t lbfgsb_data_size = sizeof(lbfgsb_data)+sizeof(double)*wa_len + sizeof(int)*3*n_max;
    lbfgsb_data * data = 
      (lbfgsb_data *)lua_newuserdata(L, lbfgsb_data_size);
    memset(data, 0, lbfgsb_data_size);
    data->wa = (double *)(data + 1);
    data->iwa = (int *)(data->wa + wa_len);  
    lua_settable(L,1);
    lua_pushstring(L, "lower");
    array_double(L, n_max);
    array * lower_array = luaL_checkudata(L,-1,"array");
    double * lower = (double *)lower_array->ptr;
    lua_settable(L,1);
    lua_pushstring(L, "upper");
    array_double(L, n_max);
    array * upper_array = luaL_checkudata(L,-1,"array");
    double * upper = (double *)upper_array->ptr;
    lua_settable(L,1);
    lua_pushstring(L, "nbd");
    array_int(L, n_max);
    array * nbd_array = luaL_checkudata(L,-1,"array");
    int * nbd = (int *)nbd_array->ptr;
    lua_settable(L,1);
    for(int i=0;i<n_max;++i) {
      lower[i]=0.0;
      upper[i]=1.0;
      nbd[i]=0;
    }
    lua_pushstring(L, "factr");
    lua_pushnumber(L, 1.0e+7);
    lua_settable(L,1);
    lua_pushstring(L, "pgtol");
    lua_pushnumber(L, 1.0e-5);  
    lua_settable(L,1);
    lua_pushstring(L, "iprint");
    lua_pushinteger(L, -1);
    lua_settable(L,1);
    lua_pushstring(L, "task");
    lua_pushstring(L, "START");
    lua_settable(L,1);
  } else {
    return luaL_error(L, "lbfgsb.new: expected table with n_max and m_max fields");
  }
  return 1;
}

static int Fcall(lua_State *L) {
  luaL_checktype(L,1,LUA_TTABLE);
  lua_getfield(L,1,"n");
  int n = luaL_checkinteger(L,-1);
  lua_pop(L,1);
  lua_getfield(L,1,"m");
  int m = luaL_checkinteger(L,-1);
  lua_pop(L,1); 
  lua_getfield(L,1,"x");
  array * x_array = luaL_checkudata(L,-1,"array");
  double * x = (double *)x_array->ptr;
  lua_pop(L,1);
  lua_getfield(L,1,"lower");
  array * lower_array = luaL_checkudata(L,-1,"array");
  double * lower = (double *)lower_array->ptr;
  lua_pop(L,1);
  lua_getfield(L,1,"upper");
  array * upper_array = luaL_checkudata(L,-1,"array");
  double * upper = (double *)upper_array->ptr;
  lua_pop(L,1);
  lua_getfield(L,1,"nbd");
  array * nbd_array = luaL_checkudata(L,-1,"array");
  int * nbd = (int *)nbd_array->ptr;
  lua_pop(L,1);
  lua_getfield(L,1,"g");
  array * grad_array = luaL_checkudata(L,-1,"array");
  double * grad = (double *)grad_array->ptr;
  lua_pop(L,1);
  lua_getfield(L,1,"f");
  double f = luaL_checknumber(L,-1);
  lua_pop(L,1);
  lua_getfield(L,1,"pgtol");
  double pgtol = luaL_checknumber(L,-1);
  lua_pop(L,1);
  lua_getfield(L,1,"factr");
  double factr = luaL_checknumber(L,-1);
  lua_pop(L,1);
  lua_getfield(L,1,"iprint");
  int iprint = luaL_checkinteger(L,-1);
  lua_pop(L,1);
  lua_getfield(L,1,"task");
  const char * task_str = luaL_checkstring(L,-1);
  lua_pop(L,1);
  lua_getfield(L,1,"__data__");
  lbfgsb_data * data = lua_touserdata(L,-1);
  lua_pop(L,1); 
  memset(data->task, ' ', 60);
  strncpy(data->task, task_str, 60);
  data->task[strlen(task_str)]=' ';
  setulb_(&n,&m,x,lower,upper,nbd,&f,grad,
    &factr,&pgtol,data->wa,
    data->iwa, data->task, &iprint, data->csave,
    data->lsave,data->isave,data->dsave,
    60,60);
  for(size_t i=0;i<60;++i) {
    if(data->task[i]==' ') {
      data->task[i]='\0';
      break;
    }
  }
  lua_pushstring(L,"task");
  lua_pushstring(L, data->task);
  lua_settable(L,1);
  lua_pushstring(L, "f");
  lua_pushnumber(L, f);
  lua_settable(L,1);
  return 1;
}
              
static const luaL_Reg R[] =
{
    { "new", Fnew },
    { "call", Fcall },
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
