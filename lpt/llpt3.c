#include <stdlib.h>
#define LIBNAME  "lpt3"
#define LIBTYPE "lpt3"
#define LIBVERSION "1"
#include "lua.h"
#include "lauxlib.h"
#define lua_swap(L) lua_insert(L, -2)
#include "../array/array.h"
#include "lpt3.h"

typedef struct luaL_IntegerConstant {
  const char *name;
  lua_Integer i;
} luaL_IntegerConstant;

static const luaL_IntegerConstant C[] = {
 {   "DIM", LPT3_DIM},
 {   "MAX_ORTHANT_LEVEL", LPT3_MAX_ORTHANT_LEVEL},
 {   NULL, 0} };


static int FInit(lua_State *L) {
  lpt3 r;
  lpt3_init(&r, luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt3_tointeger(r));
  return 1; 
} 

static int FLevel(lua_State *L) {
  lpt3 r = lpt3_frominteger(luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt3_level(r));
  return 1; 
} 

static int FSimplexLevel(lua_State *L) {
  lpt3 r = lpt3_frominteger(luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt3_simplex_level(r));
  return 1; 
} 

static int FOrthantLevel(lua_State *L) {
  lpt3 r = lpt3_frominteger(luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt3_orthant_level(r));
  return 1; 
} 

static int FChild(lua_State *L) {
  lpt3 r = lpt3_frominteger(luaL_checkinteger(L, 1));
  int zo = luaL_checkinteger(L, 2);
  r = lpt3_child(r, zo);
  lua_pushinteger(L, lpt3_tointeger(r));
  return 1; 
} 

static int FisChild0(lua_State *L) {
  lpt3 r = lpt3_frominteger(luaL_checkinteger(L, 1));
  bool b = lpt3_is_child0(r);
  lua_pushboolean(L, b);
  return 1; 
}

static int FParent(lua_State *L) {
  lpt3 r = lpt3_frominteger(luaL_checkinteger(L, 1));
  r = lpt3_parent(r);
  lua_pushinteger(L, lpt3_tointeger(r));
  return 1;
}

static int FOrientation(lua_State *L) {
  lpt3 r = lpt3_frominteger(luaL_checkinteger(L, 1));
  int o = lpt3_orientation(r);
  lua_pushinteger(L, o);
  return 1; 
}

static int FNeighborBd(lua_State *L) {
  lpt3 code = lpt3_frominteger(luaL_checkinteger(L, 1));
  int i = luaL_checkinteger(L, 2);
  lpt3 r;
  int res = lpt3_neighbor_bd(code, i, &r);
  lua_pushinteger(L, res);
  lua_pushinteger(L, lpt3_tointeger(r));
  return 2; 
}

static int FNeighbor(lua_State *L) {
  lpt3 code = lpt3_frominteger(luaL_checkinteger(L, 1));
  int i = luaL_checkinteger(L, 2);
  lpt3 r;
  bool res = lpt3_neighbor(code, i, &r);
  lua_pushboolean(L, res);
  lua_pushinteger(L, lpt3_tointeger(r));
  return 2; 
}


static int FSimplex(lua_State *L) {
  lpt3 code = lpt3_frominteger(luaL_checkinteger(L, 1));
  array *s = (array *)luaL_checkudata(L, 2, "array");
  lpt3_simplex(code, (double *)s->ptr);
  return 1;
}

static int FPrintSimplex(lua_State *L) {
  lpt3 code = lpt3_frominteger(luaL_checkinteger(L, 1));
  lpt3_print_simplex(code);
  return 0;
}

static const luaL_Reg R[] =
{
    { "init", FInit },
    { "level", FLevel },
    { "simplex_level", FSimplexLevel },
    { "orthant_level", FOrthantLevel },
    { "child", FChild },
    { "parent", FParent },
    { "is_child0", FisChild0 },
    { "orientation", FOrientation },
    { "neighbor_bd", FNeighborBd },
    { "neighbor", FNeighbor },
    { "simplex", FSimplex },
    { "print_simplex", FPrintSimplex },
    { NULL,     NULL    }
};

LUALIB_API int luaopen_lpt3(lua_State *L)
{
    luaL_newmetatable(L,LIBTYPE);
 		luaL_setfuncs(L,R,0);
    lua_pushliteral(L,"version");
    lua_pushliteral(L,LIBVERSION);
    lua_settable(L,-3);
    for(const luaL_IntegerConstant * p=C;p->name!=NULL;++p) {
        lua_pushstring(L,p->name);
        lua_pushinteger(L,p->i);
        lua_settable(L,-3);
    }
    lua_pushliteral(L,"__index");
    lua_pushvalue(L,-2);
    lua_settable(L,-3);
    return 1;
}
