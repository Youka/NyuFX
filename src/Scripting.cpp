#include "Scripting.h"
#include "libs.h"
#include <wx/thread.h>
#include <wx/msgdlg.h>

// LUA FUNCTIONS
void YieldLua(lua_State *L, lua_Debug *ar){
    if(wxThread::This()->TestDestroy())
        lua_yield(L, 0);
}

wxTextCtrl *print_target;
DEF_HEAD_NARG(print, 0)
	// Output buffer
	wxString output;
	// Through function parameters
	for(int i = 1; i <= lua_gettop(L); i++)
		switch(lua_type(L, i)){
			case LUA_TNIL:
				output << wxT("nil\n");
				break;
			case LUA_TBOOLEAN:
				output << ( lua_toboolean(L, i) ? wxT("true\n") : wxT("false\n") );
				break;
			case LUA_TLIGHTUSERDATA:
				output << wxString::Format( wxT("Lightuserdata: %d\n"), reinterpret_cast<unsigned int>(lua_touserdata(L, i)) );
				break;
			case LUA_TNUMBER:
				output << lua_tonumber(L, i) << wxT("\n");
				break;
			case LUA_TSTRING:
				output << wxString::FromUTF8(lua_tostring(L, i)) << wxT("\n");
				break;
			case LUA_TTABLE:
				output << wxString::Format( wxT("Table: %d\n"), reinterpret_cast<unsigned int>(lua_topointer(L, i)) );
				break;
			case LUA_TFUNCTION:
				output << wxString::Format( wxT("Function: %d\n"), reinterpret_cast<unsigned int>(lua_topointer(L, i)) );
				break;
			case LUA_TUSERDATA:
				output << wxString::Format( wxT("Userdata: %d\n"), reinterpret_cast<unsigned int>(lua_touserdata(L, i)) );
				break;
			case LUA_TTHREAD:
				output << wxString::Format( wxT("Thread: %d\n"), reinterpret_cast<unsigned int>(lua_topointer(L, i)) );
				break;
			default:
				output << wxT("Unknown value!\n");
		}
	// Send buffer to log
	wxMutexGuiLocker gui_locker;
	print_target->AppendText( output );
DEF_TAIL

wxGauge *progress_target;
DEF_HEAD_1ARG(progressbar, 1)
	double n = luaL_checknumber(L, 1);
	if(n >= 0 && n <= 100){
		wxMutexGuiLocker gui_locker;
		progress_target->SetValue(n);
	}else
		luaL_error2(L, "invalid number");
DEF_TAIL

// SCRIPTING DEFINITION
Scripting::Scripting(wxTextCtrl *log, wxGauge *progressbar){
	// Set Lua hook function
	lua_sethook(this->L, YieldLua, LUA_MASKCALL, 1);
	// Register Lua standard functions
	luaL_openlibs(this->L);
	// Register Lua output functions
	print_target = log;
	progress_target = progressbar;
	lua_register(this->L, "print", l_print);
	lua_getglobal(this->L, "io");
	lua_pushcfunction(this->L, l_progressbar); lua_setfield(this->L, -2, "progressbar");
	lua_pop(this->L, 1);
	// Register Lua utility functions
	luaopen_table2(this->L);
	luaopen_tgdi(this->L);
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