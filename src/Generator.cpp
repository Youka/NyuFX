#include "Generator.h"

Generator::Generator(wxString lua_file, wxString ass_file, wxString output_file, wxString command, wxTextCtrl *log, wxGauge *progressbar, wxButton *gencanc)
: lua_file(lua_file), ass_file(ass_file), output_file(output_file), command(command), log(log), progressbar(progressbar), gencanc(gencanc){
	this->gencanc->SetLabelText(_("Cancel"));
	this->gencanc->SetToolTip(_("Stop process!"));
}

Generator::~Generator(){

}

wxThread::ExitCode Generator::Entry(){






	// Execute after process command
	if(!this->command.IsEmpty())
		wxExecute(this->command);
	// Run ends
	return 0;
}