#include "Lua.h"

Lua::Lua(void){
	this->L = luaL_newstate();
	luaL_openlibs(this->L);
}

Lua::~Lua(void){
	lua_close(this->L);
}