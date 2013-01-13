#include "libs.h"
#include <wx/image.h>
#include <windows.h>

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
	if( !lua_isnumber(L, -1) || lua_tonumber(L, -1) < 1 ) luaL_error2(L, "invalid width");
	lua_getfield(L, 1, "height");
	if( !lua_isnumber(L, -1) || lua_tonumber(L, -1) < 1 ) luaL_error2(L, "invalid height");
	lua_getfield(L, 1, "has_alpha");
	if( !lua_isboolean(L, -1) ) luaL_error2(L, "invalid alpha identifier");
	int width = lua_tonumber(L, -3), height = lua_tonumber(L, -2);
	bool alpha = lua_toboolean(L, -1);
	lua_pop(L, 3);
	unsigned int pixels = alpha ? lua_objlen(L, 1) >> 2 : lua_objlen(L, 1) / 3;
	if(width * height != pixels) luaL_error2(L, "invalid data size");
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
	SetBkMode(*pdc, TRANSPARENT);
	SetPolyFillMode(*pdc, WINDING);
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(delete_context, 1)
	DeleteDC( *reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI)) );
DEF_TAIL

DEF_HEAD_1ARG(flatten_path, 1)
	if( !FlattenPath( *reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI)) ) )
		luaL_error2(L, "path flattening failed");
DEF_TAIL

DEF_HEAD_1ARG(abort_path, 1)
	if( !AbortPath( *reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI)) ) )
		luaL_error2(L, "path abortion failed");
DEF_TAIL

DEF_HEAD_1ARG(path_box, 1)
	// Get region of path
	HRGN region = PathToRegion( *reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI)) );
	if(!region)
		luaL_error2(L, "couldn't convert path to region");
	// Get bounding box
	RECT rect;
	if( !GetRgnBox(region, &rect) ){
		DeleteObject(region);
		luaL_error2(L, "couldn't get region size");
	}
	DeleteObject(region);
	// Return bounding box
	lua_pushnumber(L, rect.left);
	lua_pushnumber(L, rect.top);
	lua_pushnumber(L, rect.right);
	lua_pushnumber(L, rect.bottom);
	return 4;
DEF_TAIL

DEF_HEAD_1ARG(widen_path, 2)
	// Get context
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));
	// Set pen for widening
	const LOGBRUSH logbrush = {BS_SOLID, RGB(255, 255, 255), 0};
	HPEN pen = ExtCreatePen(
		PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND | PS_JOIN_ROUND,
		luaL_checknumber(L, 2),
		&logbrush,
		0, NULL
	);
	if(!pen)
		luaL_error2(L, "couldn't create pen");
	HGDIOBJ old_pen = SelectObject(*dc, pen);
	// Widen path
	if( !WidenPath(*dc) ){
		SelectObject(*dc, old_pen);
		DeleteObject(pen);
		luaL_error2(L, "couldn't widen path");
	}
	SelectObject(*dc, old_pen);
	DeleteObject(pen);
DEF_TAIL

DEF_HEAD_1ARG(get_path, 1)
	// Get context
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));
	// Get path size
	int n_points = GetPath(*dc, NULL, NULL, 0);
	// Create path buffers
	POINT *points = new POINT[n_points];	// allocating zero-sized array is allowed in c++
	BYTE *types = new BYTE[n_points];
	// Get path
	GetPath(*dc, points, types, n_points);
	// Path to string
	wxString path;
	BYTE type, last_type = 0x08;	// invalid type at start
	POINT point;
	for(int pi = 0; pi < n_points; pi++){
		type = types[pi];
		point = points[pi];
		if(type != last_type)
			switch(type){
				case PT_MOVETO:
					path << wxT("m ");
					break;
				case PT_LINETO:
				case PT_LINETO | PT_CLOSEFIGURE:
					path << wxT("l ");
					break;
				case PT_BEZIERTO:
				case PT_BEZIERTO | PT_CLOSEFIGURE:
					path << wxT("b ");
					break;
			}
		last_type = type;
		path << wxString::Format(wxT("%d %d "), point.x, point.y);
	}
	// Free path buffers
	delete[] points;
	delete[] types;
	// Path string to Lua
	lua_pushstring(L, path.Trim().ToAscii());
	return 1;
DEF_TAIL

DEF_HEAD_2ARG(text_extents, 4, 9)
	// Get parameters
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));	// context
	wxString text = wxString::FromUTF8(luaL_checkstring(L, 2));	// text
	const wchar_t *wtext = text.wc_str();
	wxString face = wxString::FromUTF8(luaL_checkstring(L, 3));	// font face
	const wchar_t *wface = text.wc_str();
	int size = luaL_checknumber(L, 4);	// font size
	bool bold = luaL_optboolean(L, 5, false);	// bold?
	bool italic = luaL_optboolean(L, 6, false);	// italic?
	bool underline = luaL_optboolean(L, 7, false);	// underlined?
	bool strikeout = luaL_optboolean(L, 8, false);	// strikeouted?
	BYTE charset = luaL_optnumber(L, 9, DEFAULT_CHARSET);	// charset
	// Create font
	LOGFONTW lf = {0};
	lf.lfHeight = size;
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = italic;
	lf.lfUnderline = underline;
	lf.lfStrikeOut = strikeout;
	lf.lfCharSet = charset;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfFaceName[31] = L'\0';
	wcsncpy(lf.lfFaceName, wface, 31);
	HFONT font = CreateFontIndirectW(&lf);
	if(!font)
		luaL_error2(L, "couldn't create font");
	HGDIOBJ old_font = SelectObject(*dc, font);
	// Get text extents
	SIZE sz;
	if( !GetTextExtentPoint32W(*dc, wtext, wcslen(wtext), &sz) ){
		SelectObject(*dc, old_font);
		DeleteObject(font);
		luaL_error2(L, "couldn't get text extents");
	}
	// Get font metrics
	TEXTMETRICW tm;
	if( !GetTextMetricsW(*dc, &tm) ){
		SelectObject(*dc, old_font);
		DeleteObject(font);
		luaL_error2(L, "couldn't get font metrics");
	}
	SelectObject(*dc, old_font);
	DeleteObject(font);
	// Return extents & metrics
	lua_pushnumber(L, sz.cx);
	lua_pushnumber(L, sz.cy);
	lua_pushnumber(L, tm.tmAscent);
	lua_pushnumber(L, tm.tmDescent);
	lua_pushnumber(L, tm.tmInternalLeading);
	lua_pushnumber(L, tm.tmExternalLeading);
	return 6;
DEF_TAIL

DEF_HEAD_3ARG(add_path, 1, 4, 9)
	// Shape mode
	if(lua_gettop(L) == 1){

		// TODO: insert shape path

	// Text mode
	}else{
		// Get parameters (same as in l_text_extents)
		HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));	// context
		wxString text = wxString::FromUTF8(luaL_checkstring(L, 2));	// text
		const wchar_t *wtext = text.wc_str();
		wxString face = wxString::FromUTF8(luaL_checkstring(L, 3));	// font face
		const wchar_t *wface = text.wc_str();
		int size = luaL_checknumber(L, 4);	// font size
		bool bold = luaL_optboolean(L, 5, false);	// bold?
		bool italic = luaL_optboolean(L, 6, false);	// italic?
		bool underline = luaL_optboolean(L, 7, false);	// underlined?
		bool strikeout = luaL_optboolean(L, 8, false);	// strikeouted?
		BYTE charset = luaL_optnumber(L, 9, DEFAULT_CHARSET);	// charset
		// Create font (same as in l_text_extents)
		LOGFONTW lf = {0};
		lf.lfHeight = size;
		lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;
		lf.lfItalic = italic;
		lf.lfUnderline = underline;
		lf.lfStrikeOut = strikeout;
		lf.lfCharSet = charset;
		lf.lfOutPrecision = OUT_TT_PRECIS;
		lf.lfQuality = ANTIALIASED_QUALITY;
		lf.lfFaceName[31] = L'\0';
		wcsncpy(lf.lfFaceName, wface, 31);
		HFONT font = CreateFontIndirectW(&lf);
		if(!font)
			luaL_error2(L, "couldn't create font");
		HGDIOBJ old_font = SelectObject(*dc, font);
		// Draw text
		if( !BeginPath(*dc) ){
			SelectObject(*dc, old_font);
			DeleteObject(font);
			luaL_error2(L, "couldn't begin path creation");
		}
		if( !ExtTextOutW(*dc, 0, 0, ETO_RTLREADING, NULL, wtext, wcslen(wtext), NULL) ){
			EndPath(*dc);
			SelectObject(*dc, old_font);
			DeleteObject(font);
			luaL_error2(L, "couldn't draw text path");
		}
		EndPath(*dc);
		SelectObject(*dc, old_font);
		DeleteObject(font);
	}
DEF_TAIL

// REGISTER
inline void register_tgdi_meta(lua_State *L){
	// Initialize image handlers
	if( wxImage::FindHandler(wxBITMAP_TYPE_PNG) == NULL ){
		// BMP pre-installed
		wxImage::AddHandler(new wxPNGHandler);
		wxImage::AddHandler(new wxJPEGHandler);
		wxImage::AddHandler(new wxTGAHandler);
		wxImage::AddHandler(new wxICOHandler);
	}
	// Meta methods
	luaL_newmetatable(L, TGDI);
	lua_pushcfunction(L, l_delete_context);
	lua_setfield(L, -2, "__gc");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_flatten_path);
	lua_setfield(L, -2, "flatten_path");
	lua_pushcfunction(L, l_abort_path);
	lua_setfield(L, -2, "abort_path");
	lua_pushcfunction(L, l_path_box);
	lua_setfield(L, -2, "path_box");
	lua_pushcfunction(L, l_widen_path);
	lua_setfield(L, -2, "widen_path");
	lua_pushcfunction(L, l_get_path);
	lua_setfield(L, -2, "get_path");
	lua_pushcfunction(L, l_text_extents);
	lua_setfield(L, -2, "text_extents");
	lua_pushcfunction(L, l_add_path);
	lua_setfield(L, -2, "add_path");
	lua_pop(L, 1);
}

inline void register_tgdi_lib(lua_State *L){
	const luaL_Reg tgdi[] = {
		{"load_image", l_load_image},
		{"save_png", l_save_png},
		{"create_context", l_create_context},
		{0, 0}
	};
	luaL_register(L, "tgdi", tgdi);
	lua_pop(L, 1);
}

void luaopen_tgdi(lua_State *L){
	register_tgdi_meta(L);
	register_tgdi_lib(L);
}