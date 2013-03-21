#pragma once

extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

// Lua instance
class Lua{
	public:
		// Allocate Lua instance
		Lua(){
			this->L = luaL_newstate();
			luaL_openlibs(this->L);
		}
		// Free Lua instance
		~Lua(){
			lua_close(this->L);
		}
		// Access to Lua instance
		operator lua_State*(){return this->L;}
	private:
		lua_State *L;
};

// Lua debug functions
static void luaL_error2(lua_State *L, const char *msg = 0){
	if(!msg){
		lua_Debug ar;
		lua_getstack(L, 0, &ar);
		lua_getinfo(L, "n", &ar);
		luaL_error(L, "Bad arguments to function '%s'!", ar.name);
	}else
		luaL_error(L, msg);
}

// Lua boolean functions
static int luaL_checkboolean(lua_State *L, int i){
	if(!lua_isboolean(L,i))
		luaL_typerror(L,i,"boolean");
	return lua_toboolean(L,i);
}

static int luaL_optboolean(lua_State *L, int i, int d){
	if(lua_type(L, i) < 1)
		return d;
	else
		return luaL_checkboolean(L, i);
}

// Lua userdata functions
static void *luaL_checkuserdata(lua_State *L, int i, const char* type){
	void *ud = luaL_checkudata(L, i, type);
	luaL_argcheck(L, ud, i, "wrong userdata");
	return ud;
}

template <class T> static T *lua_createuserdata(lua_State *L, const char* meta_name){
	T *ud = reinterpret_cast<T*>(lua_newuserdata(L, sizeof(T)));
	luaL_newmetatable(L, meta_name);
	lua_setmetatable(L, -2);
	return ud;
}

// Lua table functions
#include <string.h>

template <class T>
class LuaArray{
	private:
		lua_State *L;
		T *array; unsigned long int size;
	public:
		LuaArray(lua_State *L, unsigned long int sz = 0){
			this->L = L;
			this->size = sz;
			if(this->size>0){
				this->array = new T[this->size];
				memset(this->array, 0, sizeof(T) * this->size);
			}else
				this->array = 0;
		}
		~LuaArray(){
			if(this->array)
				delete[] this->array;
		}

		operator T*(){return this->array;}
		operator void*(){return reinterpret_cast<void*>(this->array);}
		void load(unsigned long int i){
			if(!lua_istable(L,i))
				luaL_typerror(L,i,"table");
			this->size = lua_objlen(L,i);
			if(this->array)
				delete[] this->array;

			if(this->size > 0){
				this->array = new T[this->size];
				for(int ii = 1; ii <= this->size; ii++){
					lua_rawgeti(L, i, ii);
					luaL_argcheck(L, lua_isnumber(L,-1), i, reinterpret_cast<char*>("invalid table"));
					this->array[ii-1] = lua_tonumber(L, -1);
					lua_pop(L,1);
				}
			}else
				this->array = 0;
		}
		void push(){
			lua_createtable(L, this->size, 0);
			for (unsigned int i = 0; i < this->size; i++){
				lua_pushnumber(L, this->array[i]);
				lua_rawseti(L, -2, i+1);
			}
		}
};

template <class T> static T *luaL_checktable(lua_State *L, int i){
	if(!lua_istable(L,i))
		luaL_typerror(L,i,"table");
	size_t len = lua_objlen(L,i);

	if(len > 0){
		T *table = new T[len];
		for(int ii = 1; ii <= len; ii++){
			lua_rawgeti(L, i, ii);
			if(!lua_isnumber(L,-1)){
				delete[] table;
				lua_pop(L,1);
				luaL_argerror(L, i, "invalid table");
			}
			table[ii-1] = lua_tonumber(L, -1);
			lua_pop(L,1);
		}
		return table;
	}else
		return 0;
}

template <class T> static void lua_pushtable(lua_State *L, T *t, unsigned long int len){
	lua_createtable(L, len, 0);
	for (unsigned int i = 0; i < len; i++){
		lua_pushnumber(L, t[i]);
		lua_rawseti(L, -2, i+1);
	}
}

// Lua macros
#define REGISTER_FUNC(func) lua_register(L, #func, l_##func)
#define REGISTER_VAR(var) lua_pushnumber(L, var); lua_setglobal(L, #var)
#define DEFINE(func) int l_##func(lua_State *L)
#define DEF_HEAD_1ARG(func, param_n) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n){
#define DEF_HEAD_2ARG(func, param_n1, param_n2) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n1 || lua_gettop(L) == param_n2){
#define DEF_HEAD_3ARG(func, param_n1, param_n2, param_n3) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n1 || lua_gettop(L) == param_n2 || lua_gettop(L) == param_n3){
#define DEF_HEAD_NARG(func, param_n) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) != param_n){
#define DEF_TAIL \
	}else \
		luaL_error2(L); \
	return 0; \
}