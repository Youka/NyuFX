#include "libs.h"
#include <windows.h>
#include <wx/image.h>

// FUNCTIONS
DEF_HEAD_1ARG(load_image, 1)
	// Get image
	wxImage img( wxString::FromUTF8(luaL_checkstring(L, 1)) );
	if(!img.IsOk())
		luaL_error2(L, "invalid image");
	// RGB data
	int width = img.GetWidth(), height = img.GetHeight(), pixels = width * height;
	unsigned char *rgb = img.GetData();
	// With alpha
	if(img.HasAlpha()){
		// Alpha data
		unsigned char *alpha = img.GetAlpha();
		// Lua data
		lua_createtable(L, pixels << 2, 3);
		// Meta
		lua_pushnumber(L, width);
		lua_setfield(L, -2, "width");
		lua_pushnumber(L, height);
		lua_setfield(L, -2, "height");
		lua_pushboolean(L, true);
		lua_setfield(L, -2, "has_alpha");
		// Pixels
		for(int pi = 0, i = 1; pi < pixels; pi++){
			lua_pushnumber(L, *rgb++);	// R
			lua_rawseti(L, -2, i++);
			lua_pushnumber(L, *rgb++);	// G
			lua_rawseti(L, -2, i++);
			lua_pushnumber(L, *rgb++);	// B
			lua_rawseti(L, -2, i++);
			lua_pushnumber(L, *alpha++);	// A
			lua_rawseti(L, -2, i++);
		}
	// Without alpha
	}else{
		// Lua data
		lua_createtable(L, pixels * 3, 3);
		// Meta
		lua_pushnumber(L, width);
		lua_setfield(L, -2, "width");
		lua_pushnumber(L, height);
		lua_setfield(L, -2, "height");
		lua_pushboolean(L, false);
		lua_setfield(L, -2, "has_alpha");
		// Pixels
		for(int pi = 0, i = 1; pi < pixels; pi++){
			lua_pushnumber(L, *rgb++);	// R
			lua_rawseti(L, -2, i++);
			lua_pushnumber(L, *rgb++);	// G
			lua_rawseti(L, -2, i++);
			lua_pushnumber(L, *rgb++);	// B
			lua_rawseti(L, -2, i++);
		}
	}
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(save_png, 2)
	// Get meta
	if( !lua_istable(L, 1) ) luaL_error2(L, "image table expected");
	lua_getfield(L, 1, "width");
	if( !lua_isnumber(L, -1) ) luaL_error2(L, "invalid width");
	lua_getfield(L, 1, "height");
	if( !lua_isnumber(L, -1) ) luaL_error2(L, "invalid height");
	lua_getfield(L, 1, "has_alpha");
	if( !lua_isboolean(L, -1) ) luaL_error2(L, "invalid alpha identifier");
	int width = lua_tonumber(L, -3), height = lua_tonumber(L, -2);
	bool alpha = lua_toboolean(L, -1);
	lua_pop(L, 3);
	unsigned int pixels = alpha ? lua_objlen(L, 1) >> 2 : lua_objlen(L, 1) / 3;
	if( pixels != width * height ) luaL_error2(L, "invalid data size");
	// Create image
	wxImage img(width, height);
	unsigned char *rgb = img.GetData();
	// Fill image with alpha
	if(alpha){
		img.InitAlpha();
		unsigned char *alpha = img.GetAlpha();
		for(unsigned int pi = 0, i = 1; pi < pixels; pi++){
			for(char ii = 0; ii < 4; ii++){
				lua_rawgeti(L, 1, i++);
				if( !lua_isnumber(L, -1) ) luaL_error2(L, "invalid data");
			}
			*rgb++ = lua_tonumber(L, -4);
			*rgb++ = lua_tonumber(L, -3);
			*rgb++ = lua_tonumber(L, -2);
			*alpha++ = lua_tonumber(L, -1);
			lua_pop(L, 4);
		}
	// Fill image without alpha
	}else{
		for(unsigned int pi = 0, i = 1; pi < pixels; pi++){
			for(char ii = 0; ii < 3; ii++){
				lua_rawgeti(L, 1, i++);
				if( !lua_isnumber(L, -1) ) luaL_error2(L, "invalid data");
			}
			*rgb++ = lua_tonumber(L, -3);
			*rgb++ = lua_tonumber(L, -2);
			*rgb++ = lua_tonumber(L, -1);
			lua_pop(L, 3);
		}
	}
	// Save image
	if( !img.SaveFile(wxString::FromUTF8(luaL_checkstring(L, 2)), wxBITMAP_TYPE_PNG) )
		luaL_error2(L, "invalid image name");
DEF_TAIL

#define TGDI "tgdi"
DEF_HEAD_1ARG(create_context, 0)
	HDC *pdc = lua_createuserdata<HDC>(L, TGDI);
	*pdc = CreateCompatibleDC(0);
	SetMapMode(*pdc, MM_TEXT);
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(delete_context, 1)
	DeleteDC( *reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI)) );
DEF_TAIL

// REGISTER
void luaopen_tgdi(lua_State *L){
	// Initialize image handlers
	if( wxImage::FindHandler(wxBITMAP_TYPE_PNG) == NULL ){
		// BMP pre-installed
		wxImage::AddHandler(new wxPNGHandler);
		wxImage::AddHandler(new wxJPEGHandler);
		wxImage::AddHandler(new wxTGAHandler);
		wxImage::AddHandler(new wxICOHandler);
	}
	// Register library
	const luaL_Reg tgdi[] = {
		{"load_image", l_load_image},
		{"save_png", l_save_png},
		{"create_context", l_create_context},
		{0, 0}
	};
	luaL_register(L, "tgdi", tgdi);
	// Register meta methods
	luaL_newmetatable(L, TGDI);
	lua_pushcfunction(L, l_delete_context);
	lua_setfield(L, -2, "__gc");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 2);
}