#include "libs.h"
#include <wx/image.h>

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
	luaL_Reg tgdi[] = {
		{0, 0}
	};
	luaL_register(L, "tgdi", tgdi);
}