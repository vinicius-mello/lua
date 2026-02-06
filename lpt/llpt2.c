#include <stdlib.h>
#define LIBNAME  "lpt2"
#define LIBTYPE "lpt2"
#define LIBVERSION "1"
#include "lua.h"
#include "lauxlib.h"
#define lua_swap(L) lua_insert(L, -2)
#include "../array/array.h"
#include "lpt2.h"

typedef struct luaL_IntegerConstant {
  const char *name;
  lua_Integer i;
} luaL_IntegerConstant;

static const luaL_IntegerConstant C[] = {
 {   "DIM", LPT2_DIM},
 {   "MAX_ORTHANT_LEVEL", LPT2_MAX_ORTHANT_LEVEL},
 {   NULL, 0} };


static int FInit(lua_State *L) {
  lpt2 r;
  lpt2_init(&r, luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt2_tointeger(r));
  return 1; 
} 

static int FLevel(lua_State *L) {
  lpt2 r = lpt2_frominteger(luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt2_level(r));
  return 1; 
} 

static int FSimplexLevel(lua_State *L) {
  lpt2 r = lpt2_frominteger(luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt2_simplex_level(r));
  return 1; 
} 

static int FOrthantLevel(lua_State *L) {
  lpt2 r = lpt2_frominteger(luaL_checkinteger(L, 1));
  lua_pushinteger(L, lpt2_orthant_level(r));
  return 1; 
} 

static int FChild(lua_State *L) {
  lpt2 r = lpt2_frominteger(luaL_checkinteger(L, 1));
  int zo = luaL_checkinteger(L, 2);
  r = lpt2_child(r, zo);
  lua_pushinteger(L, lpt2_tointeger(r));
  return 1; 
} 

static int FisChild0(lua_State *L) {
  lpt2 r = lpt2_frominteger(luaL_checkinteger(L, 1));
  bool b = lpt2_is_child0(r);
  lua_pushboolean(L, b);
  return 1; 
}

static int FParent(lua_State *L) {
  lpt2 r = lpt2_frominteger(luaL_checkinteger(L, 1));
  r = lpt2_parent(r);
  lua_pushinteger(L, lpt2_tointeger(r));
  return 1;
}

static int FOrientation(lua_State *L) {
  lpt2 r = lpt2_frominteger(luaL_checkinteger(L, 1));
  int o = lpt2_orientation(r);
  lua_pushinteger(L, o);
  return 1; 
}

static int FNeighborBd(lua_State *L) {
  lpt2 code = lpt2_frominteger(luaL_checkinteger(L, 1));
  int i = luaL_checkinteger(L, 2);
  lpt2 r;
  int res = lpt2_neighbor_bd(code, i, &r);
  lua_pushinteger(L, res);
  lua_pushinteger(L, lpt2_tointeger(r));
  return 2; 
}

static int FNeighbor(lua_State *L) {
  lpt2 code = lpt2_frominteger(luaL_checkinteger(L, 1));
  int i = luaL_checkinteger(L, 2);
  lpt2 r;
  bool res = lpt2_neighbor(code, i, &r);
  lua_pushboolean(L, res);
  lua_pushinteger(L, lpt2_tointeger(r));
  return 2; 
}


static int FSimplex(lua_State *L) {
  lpt2 code = lpt2_frominteger(luaL_checkinteger(L, 1));
  array *s = (array *)luaL_checkudata(L, 2, "array");
  lpt2_simplex(code, (double *)s->ptr);
  return 1;
}

static int FPrintSimplex(lua_State *L) {
  lpt2 code = lpt2_frominteger(luaL_checkinteger(L, 1));
  lpt2_print_simplex(code);
  return 0;
}

static int Ftree_new(lua_State *L) {
  size_t buckets = (size_t)luaL_checkinteger(L, 1);
  lpt2_tree **	t=lua_newuserdata(L,sizeof(lpt2_tree*));
  luaL_setmetatable(L,LIBTYPE);
  *t = lpt2_tree_new(buckets);
  printf("%p\n", *t);
  return 1;
}

static int Ftree_free(lua_State *L) {
  lpt2_tree **t = (lpt2_tree **)lua_touserdata(L, 1);
  lpt2_tree_free(*t);
  return 0;
}

static void add_to_table_cb(lpt2 code, void *udata) {
  lua_State *L = (lua_State *)udata;
  lua_pushinteger(L, lpt2_tointeger(code));
  lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
}

static int Ftree_search_all(lua_State *L) {
  lpt2_tree **t = (lpt2_tree **)lua_touserdata(L, 1);
  array *s = (array *)luaL_checkudata(L, 2, "array");
  lua_newtable(L);
  lpt2_tree_search_all(*t, (double *)s->ptr, add_to_table_cb, L);
  return 1;
}

static int Ftree_compat_bisect(lua_State *L) {
  lpt2_tree **t = (lpt2_tree **)lua_touserdata(L, 1);
  lpt2 code = lpt2_frominteger(luaL_checkinteger(L, 2));
  lua_newtable(L);
  lpt2_tree_compat_bisect(*t, code, add_to_table_cb, L);
  return 1;
}

static int Fleafs(lua_State *L) {
  lpt2_tree **t = (lpt2_tree **)lua_touserdata(L, 1);
  lua_newtable(L);
  lpt2_tree_visit_leafs(*t, add_to_table_cb, L);
  return 1;
} 

static int Ftree_is_leaf(lua_State *L) {
  lpt2_tree **t = (lpt2_tree **)lua_touserdata(L, 1);
  lpt2 code = lpt2_frominteger(luaL_checkinteger(L, 2));
  bool is_leaf = lpt2_tree_is_leaf(*t, code);
  lua_pushboolean(L, is_leaf);
  return 1;
}

static int Ftree_print_stats(lua_State *L) {
  lpt2_tree **t = (lpt2_tree **)lua_touserdata(L, 1);
  lpt2_tree_print_stats(*t);
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
    { "tree", Ftree_new },
    { "search_all", Ftree_search_all },
    { "compat_bisect", Ftree_compat_bisect },
    { "is_leaf", Ftree_is_leaf },
    { "leafs", Fleafs },
    { "print_stats", Ftree_print_stats },
    { "__gc", Ftree_free },
    { NULL,     NULL    }
};

LUALIB_API int luaopen_lpt2(lua_State *L)
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
