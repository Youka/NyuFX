#include "Scripting.h"
#include <wx/thread.h>
#include <wx/msgdlg.h>

// Lua functions
void YieldLua(lua_State *L, lua_Debug *ar){
    if(wxThread::This()->TestDestroy())
        lua_yield(L, 0);
}

wxTextCtrl *print_target;
wxGauge *progress_target;

// Scripting definition
Scripting::Scripting(wxTextCtrl *log, wxGauge *progressbar){
	// Set Lua output function targets
	print_target = log;
	progress_target = progressbar;
	// Extend Lua
	luaL_openlibs(this->L);
	lua_sethook(this->L, YieldLua, LUA_MASKCALL, 1);

	// TODO: add functions

}

bool Scripting::DoFile(wxString file){
	if( luaL_dofile(this->L, file.ToUTF8()) ){
		wxMessageBox( wxString::FromUTF8(lua_tostring(this->L,-1)), _("Lua error"), wxOK | wxCENTRE | wxICON_ERROR);
		return false;
	}else if(lua_status(this->L) == LUA_YIELD)
		return false;
	else
		return true;
}

bool Scripting::CallInit(unsigned int arg_n, ...){
	// Get value associated with name 'Init'
	lua_getglobal(this->L, "Init");
	// Is it a function?
	if(lua_isfunction(this->L,-1)){
		// Push function parameters
		va_list va_l;
		va_start(va_l, arg_n);
		for(unsigned int i = 0; i < arg_n; i++)
			lua_pushstring(this->L, va_arg(va_l, wxString).ToUTF8());
		va_end(va_l);
		// Call function
		if(lua_pcall(this->L, arg_n, 0, 0)){
			wxMessageBox( wxT("Init: ") + wxString::FromUTF8(lua_tostring(this->L,-1)), _("Lua error"), wxOK | wxCENTRE | wxICON_ERROR);
			return false;
		}else if(lua_status(this->L) == LUA_YIELD)
			return false;
		else
			return true;
	// Wasn't a function -> clean & error
	}else{
		lua_pop(this->L,1);
		wxMessageBox( _("'Init' function is missing!"), _("Lua error"), wxOK | wxCENTRE | wxICON_ERROR);
		return false;
	}
}

bool Scripting::CallExit(){
	// Get value associated with name 'Exit'
	lua_getglobal(this->L, "Exit");
	// Is it a function?
	if(lua_isfunction(this->L,-1)){
		// Call function
		if(lua_pcall(this->L, 0, 0, 0)){
			wxMessageBox( wxT("Exit: ") + wxString::FromUTF8(lua_tostring(this->L,-1)), _("Lua error"), wxOK | wxCENTRE | wxICON_ERROR);
			return false;
		}else if(lua_status(this->L) == LUA_YIELD)
			return false;
		else
			return true;
	// Wasn't a function -> clean & error
	}else{
		lua_pop(this->L,1);
		wxMessageBox( _("'Exit' function is missing!"), _("Lua error"), wxOK | wxCENTRE | wxICON_ERROR);
		return false;
	}
}