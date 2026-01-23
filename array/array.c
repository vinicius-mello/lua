#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <cblas.h>
#include <lapacke.h>
#include "lua.h"
#include "lauxlib.h"
#include "mycompat.h"
#include "array.h"


#define MYNAME		"array"
#define MYTYPE		MYNAME
#define MYVERSION	MYTYPE " library for " LUA_VERSION " / Jan 2026"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef struct luaL_IntegerConstant {
  const char *name;
  lua_Integer i;
} luaL_IntegerConstant;


static const luaL_IntegerConstant C[] = {
 {   "et_int", et_int},
 {   "et_uint", et_uint},
 {   "et_short", et_short},
 {   "et_ushort", et_ushort},
 {   "et_char", et_char},
 {   "et_uchar", et_uchar},
 {   "et_float", et_float},
 {   "et_double", et_double},
 {   NULL, 0} };


#define RETSIZE(t) case et_##t:\
	return sizeof(t);

static size_t size_of(entry_type entry_type) {
	switch(entry_type) {
		RETSIZE(int)
		RETSIZE(uint)
		RETSIZE(short)
		RETSIZE(ushort)
		RETSIZE(char)
		RETSIZE(uchar)
		RETSIZE(float)
		RETSIZE(double)
	}
	return 0;
}
#undef RETSIZE

#define FLOATRETCAST(t) \
		case et_##t:\
			lua_pushnumber(L, (lua_Number)*((t *)q));\
			break;

#define INTRETCAST(t) \
		case et_##t:\
			lua_pushinteger(L, (lua_Integer)*((t *)q));\
			break;

static void array_get(lua_State *L, array * a, size_t i, size_t j) {
	if(i<1 || i>a->rows || j<1 || j>a->cols) {
		luaL_error(L, "array:get: index out of bounds");
		return;
	}
	i--;j--;
	void * q = a->ptr+a->sizeof_entry*(i*a->cols+j);
	switch(a->entry_type) {
		INTRETCAST(int)
		INTRETCAST(uint)
		INTRETCAST(short)
		INTRETCAST(ushort)
		INTRETCAST(char)
		INTRETCAST(uchar)
		FLOATRETCAST(float)
		FLOATRETCAST(double)
	}
}

#undef INTRETCAST
#undef FLOATRETCAST


#define SETCAST(t) \
		case et_##t:\
			*((t *)q) = (t) n;\
			break;

static void array_set(lua_State * L, array * a, size_t i, size_t j, lua_Number n) {
	if(i<1 || i>a->rows || j<1 || j>a->cols) {
		luaL_error(L, "array:set: index out of bounds");
		return;
	}
	i--;j--;
	void * q = a->ptr+a->sizeof_entry*(i*a->cols+j);
	switch(a->entry_type) {
		SETCAST(int)
		SETCAST(uint)
		SETCAST(short)
		SETCAST(ushort)
		SETCAST(char)
		SETCAST(uchar)
		SETCAST(float)
		SETCAST(double)
	}
}

#undef SETCAST

static int Pnew(lua_State *L, size_t rows, size_t cols, entry_type entry_type)
{
	uint sizeof_entry = size_of(entry_type);
	size_t s = cols*rows*sizeof_entry;
  array *	a=lua_newuserdata(L,sizeof(array)+s);
  luaL_setmetatable(L,MYTYPE);
  a->cols = cols;
  a->rows = rows;
  a->sizeof_entry = sizeof_entry;
  a->entry_type = entry_type;
  a->ptr = a+1;
	memset(a->ptr, 0, s);
  return 1;
}

static int Pnewt(lua_State *L, entry_type entry_type)
{
	size_t rows;
	size_t cols;
	lua_pushstring(L,"rows");
	lua_gettable(L,1);
	if(lua_isinteger(L,-1)) {
		rows = lua_tointeger(L,-1);
		lua_pop(L,1);
		lua_pushstring(L,"cols");
		lua_gettable(L,1);
		if(lua_isinteger(L,-1))
			cols = lua_tointeger(L,-1);
		else cols = 1;
		lua_pop(L,1);
	} else {
		lua_pop(L,1);
		lua_pushstring(L,"cols");
		lua_gettable(L,1);
		if(lua_isinteger(L,-1)) {
			cols = lua_tointeger(L,-1);
			rows = 1;
		} else {
			rows = lua_rawlen(L,1);
			cols = 1;
		}
		lua_pop(L,1);
	}
	Pnew(L, rows, cols, entry_type);
	array * a = lua_touserdata(L,-1);
	size_t i = 1;
	size_t j = 1;
	for(size_t k=1;k<=lua_rawlen(L,1);++k) {
		lua_pushinteger(L, k);
		lua_gettable(L,1);
		lua_Number v = luaL_checknumber(L,-1);
		lua_pop(L,1);
		array_set(L, a, i, j, v);
		j+=1; 
		if(j==cols+1) { j=1; i+=1; }
		if(i==rows+1) break;
	}
	return 1;
}

#define CONSTELT(t) \
static int L##t(lua_State *L) {\
	return Pnewt(L, et_##t);\
}

CONSTELT(int)
CONSTELT(uint)
CONSTELT(short)
CONSTELT(ushort)
CONSTELT(char)
CONSTELT(uchar)
CONSTELT(float)
CONSTELT(double)

#undef CONSTELT


static int Fget(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	size_t i, j;
	if(lua_gettop(L)==2) {
		size_t index = luaL_checkint(L,2);
		i = (index-1)/a->cols + 1;
		j = (index-1)%a->cols + 1;
	} else {
		i = luaL_checkint(L,2);
		j = luaL_checkint(L,3);
	}
	array_get(L, a, i, j);
	return 1;
}

static int Fset(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	size_t i, j;
	lua_Number v;
	if(lua_gettop(L)==3) {
		size_t index = luaL_checkint(L,2);
		i = (index-1)/a->cols + 1;
		j = (index-1)%a->cols + 1;
		v = luaL_checknumber(L,3);
	} else {
		i = luaL_checkint(L,2);
		j = luaL_checkint(L,3);
		v = luaL_checknumber(L,4);
	}
	array_set(L, a, i, j, v);
	return 1;
}

static int Fadd_to_entry(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	size_t i, j;
	lua_Number v;
	if(lua_gettop(L)==3) {
		size_t index = luaL_checkint(L,2);
		i = (index-1)/a->cols + 1;
		j = (index-1)%a->cols + 1;
		v = luaL_checknumber(L,3);
	} else {
		i = luaL_checkint(L,2);
		j = luaL_checkint(L,3);
		v = luaL_checknumber(L,4);
	}
	array_get(L, a, i, j);
	v += lua_tonumber(L, -1);
	array_set(L, a, i, j, v);
	return 1;
}

static int Frows(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	lua_pushinteger(L, a->rows);
	return 1;
}

static int Fcols(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	lua_pushinteger(L, a->cols);
	return 1;
}

static int Fsize(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	lua_pushinteger(L, a->cols * a->rows);
	return 1;
}

static int Fsizeof_entry(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	lua_pushinteger(L, a->sizeof_entry);
	return 1;
}

static int Fptr(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	lua_pushlightuserdata(L, a->ptr);
	return 1;
}

static int Fentry_type(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	lua_pushinteger(L, a->entry_type);
	return 1;
}

static void Pcopy(lua_State *L, int index) {
	array * a = luaL_checkudata(L,index,MYTYPE);
	Pnew(L, a->rows, a->cols, a->entry_type);
	array * b = luaL_checkudata(L,-1,MYTYPE);
	memcpy(b->ptr, a->ptr, a->cols*a->rows*a->sizeof_entry);
}

static int Fcopy(lua_State *L) {
	Pcopy(L,1);
	return 1;
}

static int Fzero(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	memset(a->ptr, 0, a->cols*a->rows*a->sizeof_entry);
	return 1;
}

static int Fadd_to(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	array * b = luaL_checkudata(L,2,MYTYPE);
	if(a->cols != b->cols || a->rows != b->rows || a->entry_type != b->entry_type) {
		return luaL_error(L, "array:add_to: incompatible arrays");
	}
	if(a->entry_type == et_float) {
		cblas_saxpby(
			a->cols*a->rows,
			1.0,
			(float *)b->ptr, 1,
			1.0,
		(float *)a->ptr, 1
		);
	} else if(a->entry_type == et_double) {
		cblas_daxpby(
			a->cols*a->rows,
			1.0,
			(double *)b->ptr, 1,	
			1.0,
		(double *)a->ptr, 1
		);
	} else {
		return luaL_error(L, "array:add_to: unsupported entry type for add_to");
	}
	lua_pushvalue(L, 1);
	return 1;
}

static int Fadd(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	array * b = luaL_checkudata(L,2,MYTYPE);
	if(a->cols != b->cols || a->rows != b->rows || a->entry_type != b->entry_type) {
		return luaL_error(L, "array:add: incompatible arrays");
	}
	Fcopy(L);
	array * c = luaL_checkudata(L,-1,MYTYPE);
	if(a->entry_type == et_float) {
		cblas_saxpby(
			a->cols*a->rows,
			1.0,
			(float *)b->ptr, 1,
			1.0,
		(float *)c->ptr, 1
		);
	} else if(a->entry_type == et_double) {
		cblas_daxpby(
			a->cols*a->rows,
			1.0,
			(double *)b->ptr, 1,	
			1.0,
		(double *)c->ptr, 1
		);
	} else {
		return luaL_error(L, "array:add: unsupported entry type for add");
	}
	return 1;
}

static int Fsub(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	array * b = luaL_checkudata(L,2,MYTYPE);
	if(a->cols != b->cols || a->rows != b->rows || a->entry_type != b->entry_type) {
		return luaL_error(L, "array:sub: incompatible arrays");
	}
	Fcopy(L);
	array * c = luaL_checkudata(L,-1,MYTYPE);
	if(a->entry_type == et_float) {
		cblas_saxpby(
			a->cols*a->rows,
			1.0,
			(float *)b->ptr, 1,
			-1.0,
		(float *)c->ptr, 1
		);
	} else if(a->entry_type == et_double) {
		cblas_daxpby(
			a->cols*a->rows,
			1.0,
			(double *)b->ptr, 1,	
			-1.0,
		(double *)c->ptr, 1
		);
	} else {
		return luaL_error(L, "array:sub: unsupported entry type for sub");
	}
	return 1;
}

static int Fmul(lua_State *L) {
	if(lua_isnumber(L,1)) {
		lua_Number n = lua_tonumber(L,1);
		array * a = luaL_checkudata(L,2,MYTYPE);
		lua_insert(L,1);
		Fcopy(L);
		a = lua_touserdata(L,-1);
		if(a->entry_type == et_float) {
			cblas_sscal(
				a->cols*a->rows,
				(float) n,
				(float *)a->ptr, 1
			);
		} else if(a->entry_type == et_double) {
			cblas_dscal(
				a->cols*a->rows,
				(double) n,
				(double *)a->ptr, 1
			);
		} else {
			return luaL_error(L, "array:mul: unsupported entry type for mul by scalar");
		}
		return 1;
	}
	array * a = luaL_checkudata(L,1,MYTYPE);
	array * b = luaL_checkudata(L,2,MYTYPE);
	if(a->cols != b->rows || a->entry_type != b->entry_type) {
		return luaL_error(L, "array:mul: incompatible arrays");
	}
	Pnew(L, a->rows, b->cols, a->entry_type);
	array * c = luaL_checkudata(L,-1,MYTYPE);
	if(b->cols == 1) {
		if(a->entry_type == et_float) {
			cblas_sgemv(
				CblasRowMajor, CblasNoTrans,
				a->rows, a->cols,
				1.0,
				(float *)a->ptr, a->cols,
				(float *)b->ptr, 1,
				0.0,
				(float *)c->ptr, 1
			);
		} else if(a->entry_type == et_double) {
			cblas_dgemv(
				CblasRowMajor, CblasNoTrans,
				a->rows, a->cols,
				1.0,
				(double *)a->ptr, a->cols,
				(double *)b->ptr, 1,
				0.0,
				(double *)c->ptr, 1
			);
		} else {
			return luaL_error(L, "array:mul: unsupported entry type for mul");
		} 
		return 1;
	}
	if(a->entry_type == et_float) {
		cblas_sgemm(
			CblasRowMajor, CblasNoTrans, CblasNoTrans,
			a->rows, b->cols, a->cols,
			1.0,
			(float *)a->ptr, a->cols,
			(float *)b->ptr, b->cols,
			0.0,
			(float *)c->ptr, c->cols
		);
	} else if(a->entry_type == et_double) {
		cblas_dgemm(
			CblasRowMajor, CblasNoTrans, CblasNoTrans,
			a->rows, b->cols, a->cols,
			1.0,
			(double *)a->ptr, a->cols,
			(double *)b->ptr, b->cols,
			0.0,
			(double *)c->ptr, c->cols
		);
	} else {	
		return luaL_error(L, "array:mul: unsupported entry type for mul");
	}
	return 1;
}

static int Funm(lua_State *L) {
	lua_pushnumber(L, -1);
	lua_insert(L,1);
	return Fmul(L);
}

#define MIN(a,b)	((a)<(b)?(a):(b))

static void fmtnum8(char * buffer, size_t buffer_size, lua_Number n) {
	int exponent = (n==0) ? 0 : (int)log10(fabs(n));
	//int signal = (n<0 ? -1 : 1);
	if(exponent>4 || exponent<-4) {
		snprintf(buffer, buffer_size, "%8.1e", n);
		return;
	}
	snprintf(buffer, buffer_size, "%8.*g", MIN(5, 5 - exponent), n);
	buffer[8] = '\0';
}
#undef MIN

static int Ftostring(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	size_t padding = 8;
	size_t max_entries_per_line = (a->cols>6 ? 6 : a->cols);
	size_t max_lines = (a->rows>10 ? 10 : a->rows);
	bool hor_dots = (a->cols>max_entries_per_line);
	bool ver_dots = (a->rows>max_lines);
  size_t buffer_line = 2+2+(padding+1)*max_entries_per_line+(hor_dots?2:0)+1;
  {
		luaL_Buffer b;
		luaL_buffinitsize(L,&b,buffer_line);
		luaL_addstring(&b,"┌");
		for(size_t j=0;j<buffer_line-5-(hor_dots?1:0);j++)
			luaL_addchar(&b,' ');
		luaL_addstring(&b,"┐\n");
		luaL_pushresult(&b);
	}
	for(size_t i=1;i<=max_lines;++i) {
		if(i==max_lines) {
			i=a->rows;
			if(ver_dots) {
				luaL_Buffer b;
				luaL_buffinit(L,&b);
				luaL_addstring(&b,"│");
				for(size_t j=1;j<=max_entries_per_line;++j) {
					if(j==max_entries_per_line) {
						j=a->cols;
						if(hor_dots) {
							luaL_addstring(&b,"⋱");
						}
					}
					luaL_addstring(&b,"       ⋮ ");
				}
				luaL_addstring(&b,"│\n");
				luaL_pushresult(&b);
			}
		}
		luaL_Buffer b;
		luaL_buffinitsize(L,&b,buffer_line);
		luaL_addstring(&b,"│");
		for(size_t j=1;j<=max_entries_per_line;++j) {
			if(j==max_entries_per_line) {
				j=a->cols;
				if(hor_dots) {
					luaL_addstring(&b,"⋯");
				}
			}
			array_get(L, a, i, j);
			char buffer[50];
			switch (a->entry_type) {	
			case et_int:
			case et_uint:
			case et_short:
			case et_ushort:
			case et_char:
			case et_uchar:
				fmtnum8(buffer, sizeof(buffer), (lua_Number)lua_tointeger(L,-1));
				break;
			case et_float:
			case et_double:
				fmtnum8(buffer, sizeof(buffer), lua_tonumber(L,-1));
				break;
			}
			lua_pop(L,1);
			luaL_addstring(&b,buffer);
			luaL_addchar(&b,' ');
		}
		luaL_addstring(&b,"│\n");
		luaL_pushresult(&b);
	}
	{
		luaL_Buffer b;
		luaL_buffinitsize(L,&b,buffer_line);
		luaL_addstring(&b,"└");
		for(size_t j=0;j<buffer_line-5-(hor_dots?1:0);j++)
			luaL_addchar(&b,' ');
		luaL_addstring(&b,"┘");
		luaL_pushresult(&b);
	}
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%ldx%ld\n", a->rows, a->cols);
	lua_pushstring(L, buffer);		
	lua_concat(L, lua_gettop(L)-1);
	return 1;
}

static int Frandom(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	int type = luaL_checkinteger(L,2);
	array * seed_array = luaL_checkudata(L,3,MYTYPE);
	if(seed_array->cols*seed_array->rows < 4 && 
		seed_array->entry_type != et_int) {
		return luaL_error(L, "array:random: seed array must have at least 4 integer entries");
	};
	lua_pushvalue(L, 1);
	switch (a->entry_type)
	{
	case et_float:
		LAPACKE_slarnv(type, seed_array->ptr, a->cols*a->rows, (float *)a->ptr);
		break;	
	case et_double:
		LAPACKE_dlarnv(type, seed_array->ptr, a->cols*a->rows, (double *)a->ptr);
		break;	
	default:
		return luaL_error(L, "array:random: unsupported entry type for random fill");
		break;
	}
	return 1;
}

#define MIN(a,b)	((a)<(b)?(a):(b))

static int FLU_factor(lua_State *L) {
	Fcopy(L);
	array * a = luaL_checkudata(L,-1,MYTYPE);
	int info;
	Pnew(L, MIN(a->rows, a->cols), 1, et_int);
	array * p = luaL_checkudata(L,-1,MYTYPE);
	switch (a->entry_type)
	{
	case et_float:
		info = LAPACKE_sgetrf(LAPACK_ROW_MAJOR, a->rows, a->cols, (float *)a->ptr, a->cols, (int *)p->ptr);	
		break;
	case et_double:
		info = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, a->rows, a->cols, (double *)a->ptr, a->cols, (int *)p->ptr);	
		break;
	default:
		return luaL_error(L, "array:LU: unsupported entry type for LU factorization");
		break;
	}
	lua_pushinteger(L, info);
	return 3;
}
#undef MIN

static int FLU_solve(lua_State *L) {
	array * a = luaL_checkudata(L,1,MYTYPE);
	array * p = luaL_checkudata(L,2,MYTYPE);
	array * b = luaL_checkudata(L,3,MYTYPE);
	if(a->rows != a->cols || a->rows != p->rows || a->rows != b->rows) {
		return luaL_error(L, "array:LU_solve: incompatible arrays");
	}
	Pcopy(L, 3);
	array * x = luaL_checkudata(L,-1,MYTYPE);
	int info;
	switch (a->entry_type)
	{
	case et_float:
		info = LAPACKE_sgetrs(LAPACK_ROW_MAJOR, 'N', a->rows, b->cols, (float *)a->ptr, a->cols, (int *)p->ptr, (float *)x->ptr, x->cols);	
		break;
	case et_double:
		info = LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', a->rows, b->cols, (double *)a->ptr, a->cols, (int *)p->ptr, (double *)x->ptr, x->cols);	
		break;
	default:
		return luaL_error(L, "array:LU_solve: unsupported entry type for LU solve");
		break;
	}
	lua_pushinteger(L, info);
	return 2;
}

static int Findex(lua_State *L) {
	if(lua_isinteger(L,2)) {
		return Fget(L);
	}
	lua_getmetatable(L,1);
	lua_pushvalue(L,2);
	lua_rawget(L,-2);
	return 1;
}

static int Fnewindex(lua_State *L) {
	if(lua_isinteger(L,2)) {
		return Fset(L);
	}
	lua_getmetatable(L,1);
	lua_pushvalue(L,2);
	lua_pushvalue(L,3);
	lua_rawset(L,-3);
	lua_pushvalue(L,3);	
	return 1;
}

static const luaL_Reg R[] =
{
	{ "int",	Lint},
	{ "uint",	Luint},
	{ "short",	Lshort},
	{ "ushort",	Lushort},
	{ "char",	Lchar},
	{ "uchar",	Luchar},
	{ "float",	Lfloat},
	{ "double",	Ldouble},
	{ "rows",	Frows},
	{ "cols",	Fcols},
	{ "get",	Fget},
	{ "set",	Fset},
	{ "add_to_entry",	Fadd_to_entry},
	{ "size",	Fsize},
	{ "sizeof_entry",	Fsizeof_entry},
	{ "ptr",	Fptr},
	{ "entry_type",	Fentry_type},
	{ "copy",	Fcopy},
	{ "zero",	Fzero},
	{ "add_to",	Fadd_to},
	{ "add",	Fadd},
	{ "__add", Fadd},
	{ "sub",	Fsub},
	{ "__sub", Fsub},
	{ "mul",	Fmul},
	{ "__mul", Fmul},
	{ "unm",	Funm},
	{ "__unm",	Funm},
	{ "__tostring",	Ftostring},
	{ "fill_random",	Frandom},
	{ "LU_factor",	FLU_factor},
	{ "LU_solve",	FLU_solve},
	{ "__call", Fget},
	{ NULL,		NULL	}
};


LUALIB_API int luaopen_array(lua_State *L)
{
  luaL_newmetatable(L,MYTYPE);
  luaL_setfuncs(L,R,0);
  lua_pushliteral(L,"version");
  lua_pushliteral(L,MYVERSION);
  lua_settable(L,-3);
  for(const luaL_IntegerConstant * p=C;p->name!=NULL;++p) {
    lua_pushstring(L,p->name);
    lua_pushinteger(L,p->i);
    lua_settable(L,-3);
	}
 	lua_pushliteral(L,"__index");
 	//lua_pushvalue(L,-2);
 	lua_pushcfunction(L,Findex);
	lua_settable(L,-3);
	lua_pushliteral(L,"__newindex");
 	lua_pushcfunction(L,Fnewindex);
	lua_settable(L,-3);
	return 1;
}
