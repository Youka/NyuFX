#include "libs.h"
#include <windows.h>
#include <wx/image.h>

// FUNCTIONS
DEF_HEAD_1ARG(load_image, 1)
	// Get image
	wxImage img( wxString::FromUTF8(luaL_checkstring(L, 1)) );
	if(!img.IsOk())
		luaL_error2(L, "invalid image");
	// Make sure it has an alpha channel
	if( !img.HasAlpha() )
		img.InitAlpha();
	// Image data
	int width = img.GetWidth(), height = img.GetHeight();
	int count = width * height;
	unsigned char *rgb = img.GetData();
	unsigned char *alpha = img.GetAlpha();
	// Lua image data
	lua_createtable(L, count, 2);
	lua_pushnumber(L, width);
	lua_setfield(L, -2, "width");
	lua_pushnumber(L, height);
	lua_setfield(L, -2, "height");
	for(int i = 0; i < count; i++){
		lua_createtable(L, 0, 6);
		lua_pushnumber(L, *rgb++);
		lua_setfield(L, -2, "r");
		lua_pushnumber(L, *rgb++);
		lua_setfield(L, -2, "g");
		lua_pushnumber(L, *rgb++);
		lua_setfield(L, -2, "b");
		lua_pushnumber(L, *alpha++);
		lua_setfield(L, -2, "a");
		lua_pushnumber(L, i % width);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, i / width);
		lua_setfield(L, -2, "y");
		lua_rawseti(L, -2, i+1);
	}
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(save_png, 2)
	// wxString filename = wxString::FromUTF8(luaL_checkstring(L, 2));
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
		// wxImage::AddHandler(new wxBMPHandler);
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
	luaL_newmetatable(L, TGDI);
	lua_pushcfunction(L, l_delete_context);
	lua_setfield(L, -2, "__gc");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
}