#pragma once

#include "Lua.h"
#include <wx/textctrl.h>
#include <wx/gauge.h>

class Scripting{
	public:
		// Initialize environment
		Scripting(wxTextCtrl *log, wxGauge *progressbar);
		// File actions
		bool DoFile(wxString file);
		bool CallInit(unsigned int arg_n, ...);	// Arguments have to be of type wxString
		bool CallExit();
	private:
		// Lua instance
		Lua L;
};
