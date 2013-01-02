#pragma once

extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

class Lua{
	public:
		Lua();
		~Lua();
		operator lua_State*(){return this->L;}
	private:
		lua_State *L;
};
