#include <stdlib.h>
#define LIBNAME  "gl"
#define LIBTYPE "gl"
#define LIBVERSION "1"
#include "lua.h"
#include "lauxlib.h"
#define lua_swap(L) lua_insert(L, -2)
#include "../array/array.h"
#include <GL/glew.h>
typedef unsigned int uint;

typedef struct luaL_IntegerConstant {
  const char *name;
  lua_Integer i;
} luaL_IntegerConstant;

static const luaL_IntegerConstant C[] = {
 {   "BYTE", GL_BYTE},
 {   "UNSIGNED_BYTE", GL_UNSIGNED_BYTE},
 {   "SHORT", GL_SHORT},
 {   "UNSIGNED_SHORT", GL_UNSIGNED_SHORT},
 {   "INT", GL_INT},
 {   "UNSIGNED_INT", GL_UNSIGNED_INT},
 {   "FLOAT", GL_FLOAT},
 {   "DOUBLE", GL_DOUBLE},
 {   "ARRAY_BUFFER", GL_ARRAY_BUFFER},
 {   "ELEMENT_ARRAY_BUFFER", GL_ELEMENT_ARRAY_BUFFER},
 {   "STREAM_DRAW", GL_STREAM_DRAW},
 {   "STREAM_READ", GL_STREAM_READ},
 {   "STREAM_COPY", GL_STREAM_COPY},
 {   "STATIC_DRAW", GL_STATIC_DRAW},
 {   "STATIC_READ", GL_STATIC_READ},
 {   "STATIC_COPY", GL_STATIC_COPY},
 {   "DYNAMIC_DRAW", GL_DYNAMIC_DRAW},
 {   "DYNAMIC_READ", GL_DYNAMIC_READ},
 {   "DYNAMIC_COPY", GL_DYNAMIC_COPY},
 {   "LINES", GL_LINES},
 {   "LINE_LOOP", GL_LINE_LOOP},
 {   "POINT_BIT", GL_POINT_BIT},
 {   "CLIENT_VERTEX_ARRAY_BIT", GL_CLIENT_VERTEX_ARRAY_BIT},
 {   "LINE_STRIP", GL_LINE_STRIP},
 {   "LINE_BIT", GL_LINE_BIT},
 {   "TRIANGLES", GL_TRIANGLES},
 {   "TRIANGLE_STRIP", GL_TRIANGLE_STRIP},
 {   "TRIANGLE_FAN", GL_TRIANGLE_FAN},
 {   "QUADS", GL_QUADS},
 {   "QUAD_STRIP", GL_QUAD_STRIP},
 {   "VERTEX_SHADER", GL_VERTEX_SHADER},
 {   "FRAGMENT_SHADER", GL_FRAGMENT_SHADER},
 {   "GEOMETRY_SHADER", GL_GEOMETRY_SHADER},
 {  "COLOR_BUFFER_BIT", GL_COLOR_BUFFER_BIT},
 {  "DEPTH_BUFFER_BIT", GL_DEPTH_BUFFER_BIT},
 {  "STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT},
 {   NULL, 0} };

static int FInit(lua_State *L) {
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  return 0;
}

static int FGenBuffer(lua_State *L) {
	uint r;
	glGenBuffers(1,&r);
  lua_pushinteger(L,r);
	return 1;
}

static int FBindBuffer(lua_State *L) {
	uint type = luaL_checkinteger(L, 1);
	uint buffer = luaL_checkinteger(L, 2);
	glBindBuffer(type, buffer);
	return 0;
}	

static int FBufferData(lua_State *L) {
  uint type = luaL_checkinteger(L, 1);
  array * data = luaL_checkudata(L, 2, "array");
  uint change = luaL_checkinteger(L, 3);
	glBufferData(type, data->cols * data->rows * data->sizeof_entry, data->ptr, change);
	return 0;
}	

static int FVertexAttribArray(lua_State *L) {
	uint va = luaL_checkinteger(L, 1);
  array * data = luaL_checkudata(L, 2, "array");
  int norm = lua_toboolean(L, 3);
  switch (data->entry_type)
  {
  case et_float:
    	glVertexAttribPointer(va, data->cols, GL_FLOAT, norm ? GL_TRUE : GL_FALSE, 
			data->cols * sizeof(float), (void *)0); 	
    break;
  case et_double:
    	glVertexAttribPointer(va, data->cols, GL_DOUBLE, norm ? GL_TRUE : GL_FALSE, 
      data->cols * sizeof(double), (void *)0); 	
    break;
  case et_int:
    	glVertexAttribIPointer(va, data->cols, GL_INT,
      data->cols * sizeof(int), (void *)0); 	
    break;
  case et_uint:
    	glVertexAttribIPointer(va, data->cols, GL_UNSIGNED_INT,
      data->cols * sizeof(unsigned int), (void *)0); 	
    break;
  case et_short:
    	glVertexAttribIPointer(va, data->cols, GL_SHORT,
      data->cols * sizeof(short), (void *)0); 	
    break;  
  case et_ushort:
    	glVertexAttribIPointer(va, data->cols, GL_UNSIGNED_SHORT,
      data->cols * sizeof(unsigned short), (void *)0); 	
    break;      
  default:
    return luaL_error(L, "VertexAttribArray: unsupported array entry type");
    break;
  }
  return 0;
}

static int FEnableVertexAttribArray(lua_State *L) {
	uint va = luaL_checkinteger(L, 1);
	glEnableVertexAttribArray(va);
  return 0;
}

static int FGenVertexArray(lua_State *L) {
	uint r;
	glGenVertexArrays(1,&r);
  lua_pushinteger(L,r);
	return 1;
}

static int FBindVertexArray(lua_State *L) {
	uint va = luaL_checkinteger(L, 1);
	glBindVertexArray(va);
  return 0;
}	

static int FDrawArrays(lua_State *L) {
  uint type = luaL_checkinteger(L, 1);
  uint n = luaL_checkinteger(L, 2);
  uint offset = 0;
  if(lua_gettop(L)>=3) {
    offset = luaL_checkinteger(L, 3);
  }
	glDrawArrays(type, offset, n);
  return 0;
}



static int FDrawElements(lua_State *L) {
  uint type = luaL_checkinteger(L, 1);
  uint n = luaL_checkinteger(L, 2);
  uint idx_type = luaL_checkinteger(L, 3);
  uint offset = 0;
  if(lua_gettop(L)>=4) {
    offset = luaL_checkinteger(L, 4);
  }
	glDrawElements(type, n, idx_type, (void*)offset);
  return 0;
}

static int FCreateShader(lua_State *L) {
  uint shaderType = luaL_checkinteger(L, 1);
  uint shader = glCreateShader(shaderType);
  lua_pushinteger(L, shader);
  return 1; 
}

static int FDeleteShader(lua_State *L) {
  uint shader = luaL_checkinteger(L, 1);
  glDeleteShader(shader);
  return 0; 
}

static int FShaderSource(lua_State *L) {
  uint shader = luaL_checkinteger(L, 1);
  const char * src = luaL_checkstring(L, 2);
  glShaderSource(shader,1, &src,NULL);
  return 0; 
}

static int FCompileShader(lua_State *L) {
  uint shader = luaL_checkinteger(L, 1);
  glCompileShader(shader);
  return 0; 
}

static int FGetShaderInfoLog(lua_State *L) {
  uint shader = luaL_checkinteger(L, 1);
  GLint len = 0;
  GLint lenr = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
  if(len==0) {
    lua_pushstring(L, "ok");
    return 1;
  }
	luaL_Buffer b;
	luaL_buffinitsize(L,&b,len+1);	  
  char * ch = malloc(len + 1);
  glGetShaderInfoLog(shader, len, &lenr, ch);
  luaL_addstring(&b,ch);
  free(ch);
  luaL_pushresult(&b);
  return 1; 
}

static int FCreateProgram(lua_State *L) {
  uint program = glCreateProgram();
  lua_pushinteger(L, program);
  return 1; 
}

static int FDeleteProgram(lua_State *L) {
  uint program = luaL_checkinteger(L, 1);
  glDeleteProgram(program);
  return 0; 
}

static int FAttachShader(lua_State *L) {
  uint program = luaL_checkinteger(L, 1);
  uint shader = luaL_checkinteger(L, 2);
  glAttachShader(program, shader);
  return 0; 
}

static int FDetachShader(lua_State *L) {
  uint program = luaL_checkinteger(L, 1);
  uint shader = luaL_checkinteger(L, 2);
  glDetachShader(program, shader);
  return 0; 
}

static int FLinkProgram(lua_State *L) {
  uint program = luaL_checkinteger(L, 1);
  glLinkProgram(program);
  return 0; 
}

static int FUseProgram(lua_State *L) {
  uint program = luaL_checkinteger(L, 1);
  glUseProgram(program);
  return 0; 
} 

static int FGetUniformLocation(lua_State *L) {
  uint program = luaL_checkinteger(L, 1);
  const char * name = luaL_checkstring(L, 2);
  int location = glGetUniformLocation(program, name);
  lua_pushinteger(L, location);
  return 1; 
}

static int FUniform(lua_State *L) {
  int location = luaL_checkinteger(L, 1);
  if(lua_isinteger(L, 2)) {
    int value = luaL_checkinteger(L, 2);
    glUniform1i(location, value);
    return 0;
  }
  if(lua_isnumber(L, 2)) {
    float value = luaL_checknumber(L, 2);
    glUniform1f(location, value);
    return 0;
  }
  array * a = luaL_checkudata(L, 2, "array");
  switch (a->entry_type)
  {
    case et_float:
      switch(a->rows * a->cols)
      {
        case 2:
          glUniform2fv(location, a->cols, (float *)a->ptr);
          return 0; 
        case 3:
          glUniform3fv(location, a->cols, (float *)a->ptr);
          return 0; 
        case 4: 
          glUniform4fv(location, a->cols, (float *)a->ptr);
          return 0; 
        case 9:
          glUniformMatrix3fv(location, a->cols / 3, GL_FALSE, (float *)a->ptr);
          return 0;
        case 16:
          glUniformMatrix4fv(location, a->cols / 4, GL_FALSE, (float *)a->ptr);
          return 0;
        default:
          return luaL_error(L, "Uniform: unsupported array size for float");
      }
    break;
    case et_int:
      switch(a->rows * a->cols)
      {
        case 2:
          glUniform2iv(location, a->cols, (int *)a->ptr);
          return 0; 
        case 3:
          glUniform3iv(location, a->cols, (int *)a->ptr);
          return 0; 
        case 4: 
          glUniform4iv(location, a->cols, (int *)a->ptr);
          return 0; 
        default:
          return luaL_error(L, "Uniform: unsupported array size for int");
      }
    break;
    default:
      return luaL_error(L, "Uniform: unsupported array entry type");
    break;
  }
  return 0; 
}

static int FClear(lua_State *L) {
  GLbitfield mask = luaL_checkinteger(L, 1);
  glClear(mask);
  return 0; 
} 

static int FClearColor(lua_State *L) {
  GLfloat r = (GLfloat)luaL_checknumber(L, 1);
  GLfloat g = (GLfloat)luaL_checknumber(L, 2);
  GLfloat b = (GLfloat)luaL_checknumber(L, 3);
  GLfloat a = (GLfloat)luaL_checknumber(L, 4);
  glClearColor(r, g, b, a);
  return 0; 
} 

static const luaL_Reg R[] =
{
    { "Init", FInit },
    { "GenBuffer", FGenBuffer },
    { "BindBuffer", FBindBuffer },
    { "BufferData", FBufferData },
    { "VertexAttribArray", FVertexAttribArray },
    { "EnableVertexAttribArray", FEnableVertexAttribArray },
    { "GenVertexArray", FGenVertexArray },
    { "BindVertexArray", FBindVertexArray },
    { "DrawArrays", FDrawArrays },
    { "DrawElements", FDrawElements },
    { "CreateShader", FCreateShader },
    { "DeleteShader", FDeleteShader },
    { "ShaderSource", FShaderSource },
    { "CompileShader", FCompileShader },
    { "GetShaderInfoLog", FGetShaderInfoLog },
    { "CreateProgram", FCreateProgram },
    { "DeleteProgram", FDeleteProgram },
    { "AttachShader", FAttachShader },
    { "DetachShader", FDetachShader },
    { "LinkProgram", FLinkProgram },
    { "UseProgram", FUseProgram },
    { "GetUniformLocation", FGetUniformLocation },
    { "Uniform", FUniform },
    { "Clear", FClear },
    { "ClearColor", FClearColor },
    //    { "",  },
    { NULL,     NULL    }
};

LUALIB_API int luaopen_gl(lua_State *L)
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
