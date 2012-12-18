#ifndef __LUAENV__
#define __LUAENV__

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
};

//Lua utils
#include <string.h>

#define DEFINE(func) int l_ ## func (lua_State *L)
#define DEF_HEAD(func, param_n) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n){
#define DEF_HEAD2(func, param_n1, param_n2) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n1 || lua_gettop(L) == param_n2){
#define DEF_HEAD_NEG(func, param_n) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) != param_n){
#define DEF_TAIL \
    }else \
        error_msg(L); \
    return 0; \
}
#define REGISTER_FUNC(func) lua_register(L, #func, l_##func)
#define REGISTER_VAR(var) lua_pushnumber(L, var); lua_setglobal(L, #var)

static void error_msg(lua_State *L, const char *msg = NULL){
    if(!msg)
    {
        lua_Debug ar;
        lua_getstack(L, 0, &ar);
        lua_getinfo(L, "n", &ar);
        luaL_error(L, "Bad arguments to '%s' function!", ar.name);
    }else{
        luaL_error(L, msg);
    }
}

static int luaL_checkboolean(lua_State *L, int i){
	if(!lua_isboolean(L,i))
		luaL_typerror(L,i,"boolean");
	return lua_toboolean(L,i);
}

static void *luaL_checkuserdata(lua_State *L, int i, char* type){
	void *ud = luaL_checkudata(L, i, type);
	luaL_argcheck(L, ud!=NULL, i, "wrong userdata");
	return ud;
}

template <class T> static T *lua_createuserdata(lua_State *L, char *meta_name){
		T *ud = (T*)lua_newuserdata(L, sizeof(T));
		luaL_getmetatable(L, meta_name);
		lua_setmetatable(L, -2);
		return ud;
}

template <class T>
class LuaArray
{
    private:
        lua_State *L;
        T *array; unsigned long int size;
    public:
        LuaArray(lua_State *L, unsigned long int sz = 0){
            this->L = L;
            this->size = sz;
            if(size!=0){
                array = new T[size];
                memset(array, 0, sizeof(T) * size);
            }else
                array = NULL;
        }
        ~LuaArray(){delete[] array;}

        operator T*(){return this->array;}
		operator void*(){return (void*)this->array;}
        void get(unsigned long int i){
            if(!lua_istable(L,i))
                luaL_typerror(L,i,"table");
            size = lua_objlen(L,i);

            if(size > 0){
				if(array)
					delete[] array;
                array = new T[size];
                for(int ii = 1; ii <= size; ii++){
                    lua_rawgeti(L, i, ii);
                    luaL_argcheck(L, lua_isnumber(L,-1), i, (char*)"invalid table");
                    array[ii-1] = lua_tonumber(L, -1);
                    lua_pop(L,1);
                }
            }
        }
        void push(){
            lua_createtable(L, size, 0);
            for (unsigned int i = 0; i<size; i++){
                lua_pushnumber(L, array[i]);
                lua_rawseti(L, -2, i+1);
            }
        }
};

template <class T> static T *luaL_checktable(lua_State *L, int i){
	if(!lua_istable(L,i))
		luaL_typerror(L,i,"table");
	size_t len = lua_objlen(L,i);

	T *table = NULL;
	if(len > 0){
		table = new T[len];
		for(int ii = 1; ii <= len; ii++){
			lua_rawgeti(L, i, ii);
			luaL_argcheck(L, lua_isnumber(L,-1), i, (char*)"invalid table");
			table[ii-1] = lua_tonumber(L, -1);
			lua_pop(L,1);
		}
	}
	return table;
}

template <class T> static void lua_pushtable(lua_State *L, T *t, unsigned long int len){
	lua_createtable(L, len, 0);
	for (unsigned int i = 0; i<len; i++)
	{
		lua_pushnumber(L, t[i]);
		lua_rawseti(L, -2, i+1);
	}
}

#endif
