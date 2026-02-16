#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM == 501
    #define lua_rawlen(L, i) lua_objlen(L, (i))
    // Other Lua 5.1 specific compatibility definitions...
#endif

typedef struct luaL_IntegerConstant {
  const char *name;
  lua_Integer i;
} luaL_IntegerConstant;

static const luaL_IntegerConstant C[] = {
 {   "DIM", DIM},
 {   "MAX_ORTHANT_LEVEL", MAX_ORTHANT_LEVEL},
 {   NULL, 0} };


static int FInit(lua_State *L) {
  lpt r;
  LPT(init)(&r, luaL_checkinteger(L, 1));
  lua_pushinteger(L, LPT(tointeger)(r));
  return 1; 
} 

static int FLevel(lua_State *L) {
  lpt r = LPT(frominteger)(luaL_checkinteger(L, 1));
  lua_pushinteger(L, LPT(level)(r));
  return 1; 
} 

static int FSimplexLevel(lua_State *L) {
  lpt r = LPT(frominteger)(luaL_checkinteger(L, 1));
  lua_pushinteger(L, LPT(simplex_level)(r));
  return 1; 
} 

static int FOrthantLevel(lua_State *L) {
  lpt r = LPT(frominteger)(luaL_checkinteger(L, 1));
  lua_pushinteger(L, LPT(orthant_level)(r));
  return 1; 
} 

static int FChild(lua_State *L) {
  lpt r = LPT(frominteger)(luaL_checkinteger(L, 1));
  int zo = luaL_checkinteger(L, 2);
  r = LPT(child)(r, zo);
  lua_pushinteger(L, LPT(tointeger)(r));
  return 1; 
} 

static int FisChild0(lua_State *L) {
  lpt r = LPT(frominteger)(luaL_checkinteger(L, 1));
  bool b = LPT(is_child0)(r);
  lua_pushboolean(L, b);
  return 1; 
}

static int FParent(lua_State *L) {
  lpt r = LPT(frominteger)(luaL_checkinteger(L, 1));
  r = LPT(parent)(r);
  lua_pushinteger(L, LPT(tointeger)(r));
  return 1;
}

static int FOrientation(lua_State *L) {
  lpt r = LPT(frominteger)(luaL_checkinteger(L, 1));
  int o = LPT(orientation)(r);
  lua_pushinteger(L, o);
  return 1; 
}

static int FNeighborBd(lua_State *L) {
  lpt code = LPT(frominteger)(luaL_checkinteger(L, 1));
  int i = luaL_checkinteger(L, 2);
  lpt r;
  int res = LPT(neighbor_bd)(code, i, &r);
  lua_pushinteger(L, res);
  lua_pushinteger(L, LPT(tointeger)(r));
  return 2; 
}

static int FNeighbor(lua_State *L) {
  lpt code = LPT(frominteger)(luaL_checkinteger(L, 1));
  int i = luaL_checkinteger(L, 2);
  lpt r;
  bool res = LPT(neighbor)(code, i, &r);
  lua_pushboolean(L, res);
  lua_pushinteger(L, LPT(tointeger)(r));
  return 2; 
}


static int FSimplex(lua_State *L) {
  lpt code = LPT(frominteger)(luaL_checkinteger(L, 1));
  array *s = (array *)luaL_checkudata(L, 2, "array");
  LPT(simplex)(code, (double *)s->ptr);
  return 1;
}

static int FPrintSimplex(lua_State *L) {
  lpt code = LPT(frominteger)(luaL_checkinteger(L, 1));
  LPT(print_simplex)(code);
  return 0;
}

static int Ftree_new(lua_State *L) {
  size_t buckets = (size_t)luaL_checkinteger(L, 1);
  LPT(tree) ** t=lua_newuserdata(L,sizeof(LPT(tree)*));
  luaL_setmetatable(L,LIBTYPE);
  *t = LPT(tree_new)(buckets);
  return 1;
}

static int Ftree_free(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  LPT(tree_free)(*t);
  return 0;
}

static void add_to_table_cb(lpt code, void *udata) {
  lua_State *L = (lua_State *)udata;
  lua_pushinteger(L, LPT(tointeger)(code));
  lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
}

static void add_vertex_cb(int vid, double *coords, void *udata) {
  lua_State *L = (lua_State *)udata;
  lua_pushinteger(L, vid);
  lua_rawseti(L, -3, lua_rawlen(L, -3) + 1);
  lua_newtable(L);
  for(int j=0;j<DIM;++j) {
    lua_pushnumber(L, coords[j]);
    lua_rawseti(L, -2, j+1);  
  }
  lua_rawseti(L, -4, lua_rawlen(L, -4) + 1);
}

static int Ftree_search_all(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  array *s = (array *)luaL_checkudata(L, 2, "array");
  lua_newtable(L);
  LPT(tree_search_all)(*t, (double *)s->ptr, add_to_table_cb, L);
  return 1;
}

static int Ftree_compat_bisect(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  lpt code = LPT(frominteger)(luaL_checkinteger(L, 2));
  lua_newtable(L);
  lua_newtable(L);
  lua_newtable(L);
  LPT(tree_compat_bisect)(*t, code, add_to_table_cb, add_vertex_cb, L);
  lua_newtable(L);
  lua_swap(L);
  lua_pushstring(L,"subdivided");
  lua_swap(L);
  lua_settable(L, -3);
  lua_swap(L);
  lua_pushstring(L,"vertex_id");
  lua_swap(L);
  lua_settable(L, -3);
  lua_swap(L);
  lua_pushstring(L,"vertex_coord");  
  lua_swap(L);
  lua_settable(L, -3);
  return 1;
}

static int Fleafs(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  lua_newtable(L);
  LPT(tree_visit_leafs)(*t, add_to_table_cb, L);
  return 1;
} 

static int Ftree_is_leaf(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  lpt code = LPT(frominteger)(luaL_checkinteger(L, 2));
  bool is_leaf = LPT(tree_is_leaf)(*t, code);
  lua_pushboolean(L, is_leaf);
  return 1;
}

static int Ftree_print_stats(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  LPT(tree_print_stats)(*t);
  return 0;
}

static int Ftree_vertex_count(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  int count = LPT(tree_vertex_count)(*t);
  lua_pushinteger(L, count);
  return 1;
} 

static int Ftree_leaf_count(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  int count = LPT(tree_leaf_count)(*t);
  lua_pushinteger(L, count);
  return 1;
}

static int Ftree_emit_idxs(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  array *idxs = (array *)luaL_checkudata(L, 2, "array");
  LPT(tree_emit_idxs)(*t, (int *)idxs->ptr);
  return 1;
}

static int Ftree_vertex_emit_coords(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  array *coords = (array *)luaL_checkudata(L, 2, "array");
  LPT(tree_vertex_emit_coords)(*t, (double *)coords->ptr);
  return 1;
}

bool subdivide_cb(lpt code, void *udata) {
  lua_State *L = (lua_State *)udata;
  lua_pushvalue(L, -1);
  lua_pushvalue(L, -3);
  lua_pushinteger(L, LPT(tointeger)(code));
  lua_call(L, 2, 1);
  bool r = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return r;
}

static int Ftree_subdivide_while(lua_State *L) {
  LPT(tree) **t = (LPT(tree) **)lua_touserdata(L, 1);
  LPT(tree_subdivide_while)(*t, &subdivide_cb, L);
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
    { "vertex_count", Ftree_vertex_count },
    { "leaf_count", Ftree_leaf_count },
    { "emit_idxs", Ftree_emit_idxs },
    { "vertex_emit_coords", Ftree_vertex_emit_coords },
    { "subdivide_while", Ftree_subdivide_while },
    { "__gc", Ftree_free },
    { NULL,     NULL    }
};

