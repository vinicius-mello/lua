#include <stdlib.h>
#define LIBNAME  "lpt3"
#define LIBTYPE "lpt3"
#define LIBVERSION "1"
#include "lua.h"
#include "lauxlib.h"
#define lua_swap(L) lua_insert(L, -2)
#include "../array/array.h"
#include "lpt3.h"

#define LPT(x) lpt3_##x
typedef lpt3 lpt;
#define DIM LPT3_DIM
#define MAX_ORTHANT_LEVEL LPT3_MAX_ORTHANT_LEVEL

#include "llpt_inc.c"
#undef LPT

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
