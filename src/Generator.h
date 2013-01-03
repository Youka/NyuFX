#pragma once

#include <wx/thread.h>
#include <wx/textctrl.h>
#include <wx/gauge.h>
#include <wx/button.h>

class Generator : public wxThread{
	public:
		// Thread built
		Generator(wxString lua_file, wxString ass_file, wxString output_file, wxString command, wxTextCtrl *log, wxGauge *progressbar, wxButton *gencanc);
		// Thread run end
		~Generator();
	private:
		// Object references
		wxString lua_file, ass_file, output_file, command;
		wxTextCtrl *log;
		wxGauge *progressbar;
		wxButton *gencanc;
	protected:
		// Thread run
		ExitCode Entry();
};
