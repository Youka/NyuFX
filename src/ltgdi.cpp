#include "llibs.h"
#include <wx/image.h>
#include <windows.h>
#include <wx/scopedptr.h>
#include <wx/tokenzr.h>

// UTILITIES
#define TGDI "tgdi"

template<class T>
class GDIOBJ{
	private:
		T obj;
	public:
		GDIOBJ(T obj) : obj(obj){}
		~GDIOBJ(){if(obj != NULL) DeleteObject(obj);}
		operator T(){return obj;}
		bool IsOk(){return obj != NULL;}
};

inline LOGFONTW CreateLogfont(int size, bool bold, bool italic, bool underline, bool strikeout, BYTE charset, const wchar_t *wface){
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
	return lf;
}

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
		lua_pushnumber(L, width); lua_setfield(L, -2, "width");
		lua_pushnumber(L, height); lua_setfield(L, -2, "height");
		lua_pushboolean(L, true); lua_setfield(L, -2, "has_alpha");
		// Pixels
		for(int pi = 0, i = 1; pi < pixels; pi++){
			lua_pushnumber(L, *rgb++);	lua_rawseti(L, -2, i++);	// R
			lua_pushnumber(L, *rgb++); lua_rawseti(L, -2, i++);	// G
			lua_pushnumber(L, *rgb++); lua_rawseti(L, -2, i++);	// B
			lua_pushnumber(L, *alpha++); lua_rawseti(L, -2, i++);	// A
		}
	// Without alpha
	}else{
		// Lua data
		lua_createtable(L, pixels * 3, 3);
		// Meta
		lua_pushnumber(L, width); lua_setfield(L, -2, "width");
		lua_pushnumber(L, height); lua_setfield(L, -2, "height");
		lua_pushboolean(L, false); lua_setfield(L, -2, "has_alpha");
		// Pixels
		for(int pi = 0, i = 1; pi < pixels; pi++){
			lua_pushnumber(L, *rgb++); lua_rawseti(L, -2, i++);	// R
			lua_pushnumber(L, *rgb++); lua_rawseti(L, -2, i++);	// G
			lua_pushnumber(L, *rgb++); lua_rawseti(L, -2, i++);	// B
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
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(abort_path, 1)
	if( !AbortPath( *reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI)) ) )
		luaL_error2(L, "path abortion failed");
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(path_box, 1)
	// Get context
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));
	// Get region of path
	if(!SaveDC(*dc))
		luaL_error2(L, "couldn't save path on stack");
	GDIOBJ<HRGN> region(PathToRegion(*dc));
	RestoreDC(*dc, -1);
	if(!region.IsOk())
		luaL_error2(L, "couldn't convert path to region");
	// Get bounding box
	RECT rect;
	if( !GetRgnBox(region, &rect) )
		luaL_error2(L, "couldn't get region size");
	// Return bounding box
	lua_pushnumber(L, rect.left);
	lua_pushnumber(L, rect.top);
	lua_pushnumber(L, rect.right);
	lua_pushnumber(L, rect.bottom);
	return 4;
DEF_TAIL

DEF_HEAD_1ARG(widen_path, 2)
	// Get parameters
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));
	double width = luaL_checknumber(L, 2);
	if(width >= 1){
		// Set pen for widening
		const LOGBRUSH logbrush = {BS_SOLID, RGB(255, 255, 255), 0};
		GDIOBJ<HPEN> pen( ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND | PS_JOIN_ROUND, luaL_checknumber(L, 2), &logbrush, 0, NULL) );
		if(!pen.IsOk())
			luaL_error2(L, "couldn't create pen");
		SelectObject(*dc, pen);
		// Widen path
		if( !WidenPath(*dc) )
			luaL_error2(L, "couldn't widen path");
	}else
		luaL_error2(L, "invalid width");
	// Return context
	lua_pushvalue(L, 1);
	return 1;
DEF_TAIL

DEF_HEAD_1ARG(get_path, 1)
	// Get context
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));
	// Get path size
	int n_points = GetPath(*dc, NULL, NULL, 0);
	if(n_points > 0){
		// Create path buffers
		wxScopedPtr<POINT> o_points(new POINT[n_points]);
		wxScopedPtr<BYTE> o_types(new BYTE[n_points]);
		POINT *points = o_points.get();
		BYTE *types = o_types.get();
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
		// Path string to Lua
		lua_pushstring(L, path.Trim().ToAscii());
	}else
		lua_pushstring(L, "");
	return 1;
DEF_TAIL

DEF_HEAD_2ARG(text_extents, 4, 9)
	// Get parameters
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));	// context
	wxString text = wxString::FromUTF8(luaL_checkstring(L, 2));	// text
	const wchar_t *wtext = text.wc_str();
	wxString face = wxString::FromUTF8(luaL_checkstring(L, 3));	// font face
	const wchar_t *wface = face.wc_str();
	int size = luaL_checknumber(L, 4);	// font size
	bool bold = luaL_optboolean(L, 5, false);	// bold?
	bool italic = luaL_optboolean(L, 6, false);	// italic?
	bool underline = luaL_optboolean(L, 7, false);	// underlined?
	bool strikeout = luaL_optboolean(L, 8, false);	// strikeouted?
	BYTE charset = luaL_optnumber(L, 9, DEFAULT_CHARSET);	// charset
	// Create font
	LOGFONTW lf = CreateLogfont(size, bold, italic, underline, strikeout, charset, wface);
	GDIOBJ<HFONT> font( CreateFontIndirectW(&lf) );
	if(!font.IsOk())
		luaL_error2(L, "couldn't create font");
	SelectObject(*dc, font);
	// Get text extents
	SIZE sz;
	if( !GetTextExtentPoint32W(*dc, wtext, wcslen(wtext), &sz) )
		luaL_error2(L, "couldn't get text extents");
	// Get font metrics
	TEXTMETRICW tm;
	if( !GetTextMetricsW(*dc, &tm) )
		luaL_error2(L, "couldn't get font metrics");
	// Return extents & metrics
	lua_pushnumber(L, sz.cx);
	lua_pushnumber(L, sz.cy);
	lua_pushnumber(L, tm.tmAscent);
	lua_pushnumber(L, tm.tmDescent);
	lua_pushnumber(L, tm.tmInternalLeading);
	lua_pushnumber(L, tm.tmExternalLeading);
	return 6;
DEF_TAIL

DEF_HEAD_3ARG(add_path, 2, 4, 9)
	// Get context
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));
	// Retrieve old path
	wxScopedPtr<POINT> o_old_points;
	wxScopedPtr<BYTE> o_old_types;
	int n_old_points = GetPath(*dc, NULL, NULL, 0);
	if(n_old_points > 0){
		o_old_points.reset(new POINT[n_old_points]);
		o_old_types.reset(new BYTE[n_old_points]);
		GetPath(*dc, o_old_points.get(), o_old_types.get(), n_old_points);
	}
	// Shape mode
	if(lua_gettop(L) == 2){
		// Create parse buffers
		wxStringTokenizer tokens(wxString::FromUTF8(luaL_checkstring(L, 2)), wxT(" "), wxTOKEN_STRTOK);
		size_t expected_hits = tokens.CountTokens() >> 1;	// one point is two tokens, so half of all tokens is the maximum number of points
		if(expected_hits > 0){
			wxScopedPtr<POINT> o_points(new POINT[expected_hits]);
			wxScopedPtr<BYTE> o_types(new BYTE[expected_hits]);
			POINT *points = o_points.get();
			BYTE *types = o_types.get();
			// Iterate through tokens & collect points
			int n_points = 0;
			long x, y;
			BYTE current_type = PT_MOVETO;
			wxString token;
			while(tokens.HasMoreTokens()){
				token = tokens.GetNextToken();
				// Catch type
				if(token == wxT("m"))
					current_type = PT_MOVETO;
				else if(token == wxT("l"))
					current_type = PT_LINETO;
				else if(token == wxT("b"))
					current_type = PT_BEZIERTO;
				// Catch point
				else if(token.ToLong(&x)){
					if(tokens.HasMoreTokens()){
						token = tokens.GetNextToken();
						if(token.ToLong(&y)){
							points[n_points].x = x;
							points[n_points].y = y;
							types[n_points] = current_type;
							n_points++;
						}else
							luaL_error2(L, "couldn't parse the second part of a point");
					}else
						luaL_error2(L, "couldn't parse a point completely");
				}
			}
			// Draw shape
			if(n_points > 0){
				if( !BeginPath(*dc) )
					luaL_error2(L, "couldn't begin path creation");
				if(n_old_points > 0)
					if( !PolyDraw(*dc, o_old_points.get(), o_old_types.get(), n_old_points) ){
						EndPath(*dc);
						luaL_error2(L, "couldn't restore old path");
					}
				if( !PolyDraw(*dc, points, types, n_points) ){
						EndPath(*dc);
					luaL_error2(L, "couldn't draw shape");
				}
				EndPath(*dc);
			}
		}
	// Text mode
	}else{
		// Get parameters (same as in l_text_extents, except context)
		wxString text = wxString::FromUTF8(luaL_checkstring(L, 2));	// text
		const wchar_t *wtext = text.wc_str();
		wxString face = wxString::FromUTF8(luaL_checkstring(L, 3));	// font face
		const wchar_t *wface = face.wc_str();
		int size = luaL_checknumber(L, 4);	// font size
		bool bold = luaL_optboolean(L, 5, false);	// bold?
		bool italic = luaL_optboolean(L, 6, false);	// italic?
		bool underline = luaL_optboolean(L, 7, false);	// underlined?
		bool strikeout = luaL_optboolean(L, 8, false);	// strikeouted?
		BYTE charset = luaL_optnumber(L, 9, DEFAULT_CHARSET);	// charset
		// Create font (same as in l_text_extents)
		LOGFONTW lf = CreateLogfont(size, bold, italic, underline, strikeout, charset, wface);
		GDIOBJ<HFONT> font( CreateFontIndirectW(&lf) );
		if(!font.IsOk())
			luaL_error2(L, "couldn't create font");
		SelectObject(*dc, font);
		// Draw text
		if( !BeginPath(*dc) )
			luaL_error2(L, "couldn't begin path creation");
		if(n_old_points > 0)
			if( !PolyDraw(*dc, o_old_points.get(), o_old_types.get(), n_old_points) ){
				EndPath(*dc);
				luaL_error2(L, "couldn't restore old path");
			}
		if( !ExtTextOutW(*dc, 0, 0, 0x0, NULL, wtext, wcslen(wtext), NULL) ){
			EndPath(*dc);
			luaL_error2(L, "couldn't draw text path");
		}
		EndPath(*dc);
	}
	// Return context
	lua_pushvalue(L, 1);
	return 1;
DEF_TAIL

DEF_HEAD_2ARG(get_pixels, 1, 2)
	// Get parameters
	HDC *dc = reinterpret_cast<HDC*>(luaL_checkuserdata(L, 1, TGDI));
	bool div8 = luaL_optboolean(L, 2, false);
	// Get region of path
	if(!SaveDC(*dc))
		luaL_error2(L, "couldn't save path on stack");
	GDIOBJ<HRGN> region(PathToRegion(*dc));
	RestoreDC(*dc, -1);
	if(!region.IsOk())
		luaL_error2(L, "couldn't convert path to region");
	// Get dimension
	RECT rect;
	if( !GetRgnBox(region, &rect) )
		luaL_error2(L, "couldn't get region size");
	int width = rect.right + 0x8 - (rect.right & 0x7), height = rect.bottom + 0x8 - (rect.bottom & 0x7), pixels = width * height;	// Bitmap size have to be 8-fold
	// Create bitmap
	const BITMAPINFO bmp_info = {
		{
			sizeof(BITMAPINFOHEADER),	// biSize
			width,	// biWidth
			-height,	// biHeight
			1,	// biPlanes
			24,	// biBitCount
			BI_RGB,	// biCompression
			0	// biSizeImage / biXPelsPerMeter / biYPelsPerMeter / biClrUsed / biClrImportant
		},
		NULL	// bmiColors
	};
	BYTE *bmp_data;
	GDIOBJ<HBITMAP> bmp( CreateDIBSection(*dc, &bmp_info, DIB_RGB_COLORS, reinterpret_cast<void**>(&bmp_data), NULL, 0) );
	if(!bmp.IsOk())
		luaL_error2(L, "couldn't create bitmap");
	SelectObject(*dc, bmp);
	// Draw region
	if( !FillRgn(*dc, region, reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH))) )
		luaL_error2(L, "couldn't draw region");
	if( !GdiFlush() )
		luaL_error2(L, "couldn't flush batch");
	// Pixels to Lua
	if(div8){
		lua_createtable(L, pixels >> 6, 2);
		lua_pushnumber(L, width >> 3); lua_setfield(L, -2, "width");
		lua_pushnumber(L, height >> 3); lua_setfield(L, -2, "height");
		for(int y = 0, i = 1, alpha; y < height; y+=8)
			for(int x = 0; x < width; x+=8){
				// Convert 8x8 pixel block to one transparent pixel
				alpha = 0;
				for(unsigned char yy = 0; yy < 8; yy++)
					for(unsigned char xx = 0; xx < 8; xx++)
						alpha += *(bmp_data + (y + yy) * width * 3 + (x + xx) * 3);
				// Insert pixel
				lua_pushnumber(L, alpha >> 6); lua_rawseti(L, -2, i++);
			}
	}else{
		lua_createtable(L, pixels, 2);
		lua_pushnumber(L, width); lua_setfield(L, -2, "width");
		lua_pushnumber(L, height); lua_setfield(L, -2, "height");
		for(int pi = 0, i = 1; pi < pixels; pi++, bmp_data+=3){
			// Insert pixel
			lua_pushboolean(L, *bmp_data); lua_rawseti(L, -2, i++);
		}
	}
	return 1;
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
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_delete_context); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_flatten_path); lua_setfield(L, -2, "flatten_path");
	lua_pushcfunction(L, l_abort_path); lua_setfield(L, -2, "abort_path");
	lua_pushcfunction(L, l_path_box); lua_setfield(L, -2, "path_box");
	lua_pushcfunction(L, l_widen_path); lua_setfield(L, -2, "widen_path");
	lua_pushcfunction(L, l_get_path); lua_setfield(L, -2, "get_path");
	lua_pushcfunction(L, l_text_extents); lua_setfield(L, -2, "text_extents");
	lua_pushcfunction(L, l_add_path); lua_setfield(L, -2, "add_path");
	lua_pushcfunction(L, l_get_pixels); lua_setfield(L, -2, "get_pixels");
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