#include "libs.h"

// FUNCTIONS
DEF_HEAD_1ARG(table_create, 2)
	double arraysize = luaL_checknumber(L, 1), mapsize = luaL_checknumber(L, 2);
	if(arraysize >= 0 && mapsize >= 0){
		lua_createtable(L, arraysize, mapsize);
		return 1;
	}else
		luaL_error2(L, "sizes have to be bigger-equal zero");
DEF_TAIL

// REGISTER
void luaopen_table2(lua_State *L){
	// Add function
	lua_getglobal(L, "table");
	if(lua_istable(L, -1)){
		lua_pushcfunction(L, l_table_create); lua_setfield(L, -2, "create");
		lua_pop(L, 1);
	}else{
		lua_pop(L, 1);
		lua_createtable(L, 0, 1);
		lua_pushcfunction(L, l_table_create); lua_setfield(L, -2, "create");
		lua_setglobal(L, "table");
	}
}