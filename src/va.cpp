#include "libs.h"

void luaopen_va(lua_State *L){
	// Register library
	luaL_Reg va[] = {
		{0, 0}
	};
	luaL_register(L, "va", va);
}