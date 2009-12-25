// Copyright 2009 Phoenix Sol - phoenix@burninglabs.com - MIT License //

#include <ketama.h>
#define LIB_NAME "ketama"
#define MT_NAME "KETAMA_MT"


typedef struct
{
	ketama_continuum *cont;
} contdata;

static int
nil_error(lua_State *L, int code, const char *msg){
   lua_pushnil(L);
   lua_pushstring(L, msg);
   lua_pushinteger(L, code);
   return 3;
}

static int
bool_error(lua_State *L, int code, const char *msg){
   lua_pushboolean(L, 0);
   lua_pushstring(L, msg);
   lua_pushinteger(L, code);
   return 3;
}

static int
default_error(lua_State *L){
   return nil_error(L, errno, strerror(errno));
}

static int
lketama_error(lua_State *L, TCRDB *rdb){
	return bool_error(L, 0, ketama_error());
}

static int
lketama_roll(lua_State *L){
	size_t fsiz;
	const char *filename = luaL_checklstring(L, 1, &fsiz);
	if( fsiz > 255 ) return luaL_argerror(L, 1, "filename must not be longer than 255 bytes");
	contdata *data = lua_newuserdata(L, sizeof(data));
	if(!data) return default_error(L);
	if(ketama_roll(data->cont, filename) == 0) return nil_error(L, 0, "continuum fail");
	luaL_getmetatable(L, MT_NAME);
   lua_setmetatable(L, -2);
   return 1;
}

ketama_continuum*
lketama_get(lua_State *L, index){
	contdata *data = luaL_checkudata(L, index, MT_NAME);
   return (ketama_continuum *)data->cont;
}

static int
lketama_smoke(lua_State *L){
	contdata *data = luaL_checkudata(L, 1, MT_NAME);
	if(data->cont) ketama_smoke(data->cont);
	return 0;
}

static int
lketama_get_server(lua_State *L){
	ketama_continuum *cont = lketama_get(L, 1);
	const char *key = luaL_checkstring(L, 2);
	mcs *result = ketama_get_server(key, cont);
	lua_pushstring(L, mcs->ip);
	lua_pushnumber(L, mcs->point);
	return 2;
}

static int
lketama_print_continuum(lua_State *L){
	ketama_continuum *cont = lketama_get(L, 1);
	ketama_print_continuum(cont);
	return 0;
}

static int
lketama_compare(lua_State *L){
	ketama_continuum *cont = lketama_get(L, 1);
	const char *ipa = lua_checkstring(L, 2);
	const char *ipb = lua_checkstring(L, 3);
	mcs *mcsa = ketama_get_server(ipa, cont);
	mcs *mcsb = ketama_get_server(ipb, cont);
	int res = ketama_compare(mcsa, mcsb);
	lua_pushnumber(L, res);
	return 1;
}

static int
lketama_hashi(lua_State *L){
	ketama_continuum *cont = lketama_get(L, 1);
	const char *str = lua_checkstring(L, 2);
	unsigned int res = ketama_hashi(str);
	lua_pushnumber(L, res);
	return 1;
}

static int
lketama_md5digest(lua_State *L){
	ketama_continuum *cont = lketama_get(L, 1);
	const char *str = lua_checkstring(L, 2);
	unsigned char md5pword[16];
	ketama_md5_digest(str, md5pword);
	lua_pushlstring(L, md5pword, 16);
	return 1;
}

static luaL_reg pfuncs[] = { {"open", lketama_roll}, {NULL, NULL} };
static luaL_reg mmethods[] = {
	{"close",				lketama_smoke},
	{"get_server",			lketama_get_server},
	{"print_continuum",	lketama_print_continuum},
	{"compare",				lketama_compare},
	{"hashi",				lketama_hashi},
	{"md5digest",			lketama_md5digest},
	{NULL,					NULL}
};

#define register_constant(s) lua_pushinteger(L,s); lua_setfield(L, -2, #s);
int luaopen_tokyotyrant(lua_State *L){
	luaL_newmetatable(L, MT_NAME);
	lua_createtable(L, 0, sizeof(mmethods) / sizeof(luaL_reg) -1);
	luaL_register(L, NULL, mmethods);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, lketama_smoke);
	lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);

	luaL_register(L, LIB_NAME, pfuncs);
	return 1;
}
