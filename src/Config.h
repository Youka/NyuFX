#pragma once

#include <wx/string.h>

namespace Config{
	// Get configuration
	wxString* LuaInput();
	wxString* ASSInput();
	wxString* ASSOutput();
	wxString* Command();
	bool AutoRun();
	wxString* Language();
	wxString* Sound();
	wxString* Font();
	unsigned long* FontSize();
	bool* Minimize2Icon();

	// Load configuration from config file & command line
	void Load();
	// Save configuration to config file
	void Save();
}