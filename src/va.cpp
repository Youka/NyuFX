#include "libs.h"

// FUNCTIONS

// REGISTER
inline void register_va_meta(lua_State *L){

}

inline void register_va_lib(lua_State *L){
	luaL_Reg va[] = {
		{0, 0}
	};
	luaL_register(L, "va", va);
	lua_pop(L, 1);
}

void luaopen_va(lua_State *L){
	register_va_meta(L);
	register_va_lib(L);
}