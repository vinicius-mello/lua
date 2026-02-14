/*
* lpdfsg.c
* pdfsg library for Lua
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 08 May 2025 09:42:40
* This code is hereby placed in the public domain and also under the MIT license
*/

#include "lua.h"
#include "lauxlib.h"
#include "mycompat.h"

#include "pdfsg.h"

#define MYNAME		"pdfsg"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / May 2025"
#define MYTYPE		MYNAME " handle"

static struct pdf_doc *getpdf(lua_State *L, int i)
{
 return luaL_unboxpointer(L,i,MYTYPE);
}

static int Lnewdoc(lua_State *L)		/** newdoc(filename) */
{
 const char *filename = luaL_checkstring(L,1);
 struct pdf_doc *pdf = pdf_newdoc(filename);
 if (pdf==NULL)
  lua_pushnil(L);
 else
 {
  luaL_boxpointer(L,pdf);
  luaL_setmetatable(L,MYTYPE);
 }
 return 1;
}

static int Lnewpage(lua_State *L)		/** newpage(width,height,label) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float width = luaL_checknumber(L,2);
 float height = luaL_checknumber(L,3);
 const char *label = luaL_checkstring(L,4);
 int rc = pdf_newpage(pdf,width,height,label);
 lua_pushinteger(L,rc);
 return 1;
}

static int Lsetinfo(lua_State *L)		/** setinfo(info) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 const char *info = luaL_checkstring(L,2);
 pdf_setinfo(pdf,info);
 lua_settop(L,1);
 return 1;
}

static int Lsetfont(lua_State *L)		/** setfont(font) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 const char *font = luaL_checkstring(L,2);
 int rc = pdf_setfont(pdf,font);
 lua_pushinteger(L,rc);
 return 1;
}

static int Lenddoc(lua_State *L)		/** enddoc() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 int rc = pdf_enddoc(pdf);
 lua_pushinteger(L,rc);
 return 1;
}

static int Lendgroup(lua_State *L)		/** endgroup() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_endgroup(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lnewgroup(lua_State *L)		/** newgroup() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 int rc = pdf_newgroup(pdf);
 lua_pushinteger(L,rc);
 return 1;
}

static int Laddgroup(lua_State *L)		/** addgroup(p,g) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 int p = luaL_checkinteger(L,2);
 int g = luaL_checkinteger(L,3);
 pdf_addgroup(pdf,p,g);
 lua_settop(L,1);
 return 1;
}

static int Lconcat(lua_State *L)		/** concat(a,b,c,d,x,y) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float a = luaL_checknumber(L,2);
 float b = luaL_checknumber(L,3);
 float c = luaL_checknumber(L,4);
 float d = luaL_checknumber(L,5);
 float x = luaL_checknumber(L,6);
 float y = luaL_checknumber(L,7);
 pdf_concat(pdf,a,b,c,d,x,y);
 lua_settop(L,1);
 return 1;
}

static int Lmoveto(lua_State *L)		/** moveto(x1,y1) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x1 = luaL_checknumber(L,2);
 float y1 = luaL_checknumber(L,3);
 pdf_moveto(pdf,x1,y1);
 lua_settop(L,1);
 return 1;
}

static int Llineto(lua_State *L)		/** lineto(x1,y1) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x1 = luaL_checknumber(L,2);
 float y1 = luaL_checknumber(L,3);
 pdf_lineto(pdf,x1,y1);
 lua_settop(L,1);
 return 1;
}

static int Lcurveto(lua_State *L)		/** curveto(x1,y1,x2,y2,x3,y3) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x1 = luaL_checknumber(L,2);
 float y1 = luaL_checknumber(L,3);
 float x2 = luaL_checknumber(L,4);
 float y2 = luaL_checknumber(L,5);
 float x3 = luaL_checknumber(L,6);
 float y3 = luaL_checknumber(L,7);
 pdf_curveto(pdf,x1,y1,x2,y2,x3,y3);
 lua_settop(L,1);
 return 1;
}

static int Lconicto(lua_State *L)		/** conicto(x1,y1,x2,y2) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x1 = luaL_checknumber(L,2);
 float y1 = luaL_checknumber(L,3);
 float x2 = luaL_checknumber(L,4);
 float y2 = luaL_checknumber(L,5);
 pdf_conicto(pdf,x1,y1,x2,y2);
 lua_settop(L,1);
 return 1;
}

static int Lrectangle(lua_State *L)		/** rectangle(x,y,width,height) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x = luaL_checknumber(L,2);
 float y = luaL_checknumber(L,3);
 float width = luaL_checknumber(L,4);
 float height = luaL_checknumber(L,5);
 pdf_rectangle(pdf,x,y,width,height);
 lua_settop(L,1);
 return 1;
}

static int Lclosepath(lua_State *L)		/** closepath() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_closepath(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lfill(lua_State *L)			/** fill() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_fill(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lstroke(lua_State *L)		/** stroke() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_stroke(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lfillstroke(lua_State *L)		/** fillstroke() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_fillstroke(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lsetopacity(lua_State *L)		/** setopacity(v) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float v = luaL_checknumber(L,2);
 pdf_setopacity(pdf,v);
 lua_settop(L,1);
 return 1;
}

static int Lsetfill(lua_State *L)		/** setfill(r,g,b) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float r = luaL_checknumber(L,2);
 float g = luaL_checknumber(L,3);
 float b = luaL_checknumber(L,4);
 pdf_setfill(pdf,r,g,b);
 lua_settop(L,1);
 return 1;
}

static int Lsetstroke(lua_State *L)		/** setstroke(r,g,b) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float r = luaL_checknumber(L,2);
 float g = luaL_checknumber(L,3);
 float b = luaL_checknumber(L,4);
 pdf_setstroke(pdf,r,g,b);
 lua_settop(L,1);
 return 1;
}

static int Lsetgray(lua_State *L)		/** setgray(f,s) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float f = luaL_checknumber(L,2);
 float s = luaL_checknumber(L,3);
 pdf_setgray(pdf,f,s);
 lua_settop(L,1);
 return 1;
}

static int Lsetlinewidth(lua_State *L)		/** setlinewidth(w) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float w = luaL_checknumber(L,2);
 pdf_setlinewidth(pdf,w);
 lua_settop(L,1);
 return 1;
}

static int Lsetlinejoin(lua_State *L)		/** setlinejoin(v) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 int v = luaL_checkinteger(L,2);
 pdf_setlinejoin(pdf,v);
 lua_settop(L,1);
 return 1;
}

static int Lsetlinecap(lua_State *L)		/** setlinecap(v) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 int v = luaL_checkinteger(L,2);
 pdf_setlinecap(pdf,v);
 lua_settop(L,1);
 return 1;
}

static int Lsetmiterlimit(lua_State *L)	/** setmiterlimit(v) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 int v = luaL_checkinteger(L,2);
 pdf_setmiterlimit(pdf,v);
 lua_settop(L,1);
 return 1;
}

#if 0
static int Lsetdash(lua_State *L)		/** setdash(n,d) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 //float a[] = luaL_checknumber(L,2);
 int n = luaL_checkinteger(L,1);
 int d = luaL_checkinteger(L,1);
 //pdf_setdash(pdf,n,d);
 lua_settop(L,1);
 return 1;
}
#endif

static int Lclip(lua_State *L)			/** clip() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_clip(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lsave(lua_State *L)			/** save() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_save(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lrestore(lua_State *L)		/** restore() */
{
 struct pdf_doc *pdf = getpdf(L,1);
 pdf_restore(pdf);
 lua_settop(L,1);
 return 1;
}

static int Lsetcolor(lua_State *L)		/** setcolor(r,g,b,a) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float r = luaL_checknumber(L,2);
 float g = luaL_checknumber(L,3);
 float b = luaL_checknumber(L,4);
 float a = luaL_checknumber(L,5);
 pdf_setcolor(pdf,r,g,b,a);
 lua_settop(L,1);
 return 1;
}

static int Lscale(lua_State *L)		/** scale(x,y) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x = luaL_checknumber(L,2);
 float y = luaL_checknumber(L,3);
 pdf_scale(pdf,x,y);
 lua_settop(L,1);
 return 1;
}

static int Ltranslate(lua_State *L)		/** translate(x,y) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x = luaL_checknumber(L,2);
 float y = luaL_checknumber(L,3);
 pdf_translate(pdf,x,y);
 lua_settop(L,1);
 return 1;
}

static int Lrotate(lua_State *L)		/** rotate(angle) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float angle = luaL_checknumber(L,2);
 pdf_rotate(pdf,angle);
 lua_settop(L,1);
 return 1;
}

static int Lline(lua_State *L)			/** line(x1,y1,x2,y2) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x1 = luaL_checknumber(L,2);
 float y1 = luaL_checknumber(L,3);
 float x2 = luaL_checknumber(L,4);
 float y2 = luaL_checknumber(L,5);
 pdf_line(pdf,x1,y1,x2,y2);
 lua_settop(L,1);
 return 1;
}

static int Ltriangle(lua_State *L)		/** triangle(x1,y1,x2,y2,x3,y3) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x1 = luaL_checknumber(L,2);
 float y1 = luaL_checknumber(L,3);
 float x2 = luaL_checknumber(L,4);
 float y2 = luaL_checknumber(L,5);
 float x3 = luaL_checknumber(L,6);
 float y3 = luaL_checknumber(L,7);
 pdf_triangle(pdf,x1,y1,x2,y2,x3,y3);
 lua_settop(L,1);
 return 1;
}

static int Lquad(lua_State *L)			/** quad(x1,y1,x2,y2,x3,y3,x4,y4) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x1 = luaL_checknumber(L,2);
 float y1 = luaL_checknumber(L,3);
 float x2 = luaL_checknumber(L,4);
 float y2 = luaL_checknumber(L,5);
 float x3 = luaL_checknumber(L,6);
 float y3 = luaL_checknumber(L,7);
 float x4 = luaL_checknumber(L,8);
 float y4 = luaL_checknumber(L,9);
 pdf_quad(pdf,x1,y1,x2,y2,x3,y3,x4,y4);
 lua_settop(L,1);
 return 1;
}

#if 0
static int Lpolyline(lua_State *L)		/** polyline(n) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 //float x[] = luaL_checknumber(L,1);
 //float y[] = luaL_checknumber(L,1);
 int n = luaL_checkinteger(L,1);
 //pdf_polyline(pdf,n);
 lua_settop(L,1);
 return 1;
}

static int Lpolygon(lua_State *L)		/** polygon(n) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 //float x[] = luaL_checknumber(L,1);
 //float y[] = luaL_checknumber(L,1);
 int n = luaL_checkinteger(L,1);
 //pdf_polygon(pdf,n);
 lua_settop(L,1);
 return 1;
}
#endif

static int Lellipse(lua_State *L)		/** ellipse(x,y,xr,yr) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x = luaL_checknumber(L,2);
 float y = luaL_checknumber(L,3);
 float xr = luaL_checknumber(L,4);
 float yr = luaL_checknumber(L,5);
 pdf_ellipse(pdf,x,y,xr,yr);
 lua_settop(L,1);
 return 1;
}

static int Lcircle(lua_State *L)		/** circle(x,y,r) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 float x = luaL_checknumber(L,2);
 float y = luaL_checknumber(L,3);
 float r = luaL_checknumber(L,4);
 pdf_circle(pdf,x,y,r);
 lua_settop(L,1);
 return 1;
}

static int Ltext(lua_State *L)			/** text(text,font,size,x,y,angle) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 const char *text = luaL_checkstring(L,2);
 int font = luaL_checkinteger(L,3);
 float size = luaL_checknumber(L,4);
 float x = luaL_checknumber(L,5);
 float y = luaL_checknumber(L,6);
 float angle = luaL_optnumber(L,7,0);
 int mode = luaL_optinteger(L,8,0);
 pdf_text(pdf,text,font,size,x,y,angle,mode);
 lua_settop(L,1);
 return 1;
}

static int Laddraw(lua_State *L)		/** addraw(code) */
{
 struct pdf_doc *pdf = getpdf(L,1);
 const char *code = luaL_checkstring(L,2);
 pdf_addraw(pdf,code);
 lua_settop(L,1);
 return 1;
}

static const luaL_Reg R[] =
{
	{ "addgroup",		Laddgroup},
	{ "addraw",		Laddraw},
	{ "circle",		Lcircle},
	{ "clip",		Lclip},
	{ "closepath",		Lclosepath},
	{ "concat",		Lconcat},
	{ "conicto",		Lconicto},
	{ "curveto",		Lcurveto},
	{ "ellipse",		Lellipse},
	{ "enddoc",		Lenddoc},
	{ "endgroup",		Lendgroup},
	{ "fill",		Lfill},
	{ "fillstroke",		Lfillstroke},
	{ "line",		Lline},
	{ "lineto",		Llineto},
	{ "moveto",		Lmoveto},
	{ "newdoc",		Lnewdoc},
	{ "newgroup",		Lnewgroup},
	{ "newpage",		Lnewpage},
#if 0
	{ "polygon",		Lpolygon},
	{ "polyline",		Lpolyline},
#endif
	{ "quad",		Lquad},
	{ "rectangle",		Lrectangle},
	{ "restore",		Lrestore},
	{ "rotate",		Lrotate},
	{ "save",		Lsave},
	{ "scale",		Lscale},
	{ "setcolor",		Lsetcolor},
#if 0
	{ "setdash",		Lsetdash},
#endif
	{ "setfill",		Lsetfill},
	{ "setfont",		Lsetfont},
	{ "setgray",		Lsetgray},
	{ "setinfo",		Lsetinfo},
	{ "setlinecap",		Lsetlinecap},
	{ "setlinejoin",	Lsetlinejoin},
	{ "setlinewidth",	Lsetlinewidth},
	{ "setmiterlimit",	Lsetmiterlimit},
	{ "setopacity",		Lsetopacity},
	{ "setstroke",		Lsetstroke},
	{ "stroke",		Lstroke},
	{ "text",		Ltext},
	{ "translate",		Ltranslate},
	{ "triangle",		Ltriangle},
	{ NULL,			NULL	}
};

LUALIB_API int luaopen_pdfsg(lua_State *L)
{
 luaL_newmetatable(L,MYTYPE);
 luaL_setfuncs(L,R,0);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 return 1;
}
