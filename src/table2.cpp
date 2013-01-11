#include "libs.h"

// FUNCTIONS
DEF_HEAD_1ARG(table_create, 2)
	lua_createtable(L, luaL_checknumber(L, 1), luaL_checknumber(L, 2));
	return 1;
DEF_TAIL

// REGISTER
void luaopen_table2(lua_State *L){
	// Add function
	lua_getglobal(L, "table");
	if(lua_istable(L, -1)){
		lua_pushcfunction(L, l_table_create);
		lua_setfield(L, -2, "create");
	}
	lua_pop(L, 1);
}