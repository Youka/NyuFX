#include "Generator.h"
#include <wx/sound.h>

Generator::Generator(wxString lua_file, wxString ass_file, wxString output_file, wxString command, wxString sound, wxTextCtrl *log, wxGauge *progressbar, wxButton *gencanc)
: lua_file(lua_file), ass_file(ass_file), output_file(output_file), command(command), sound(sound), log(log), progressbar(progressbar), gencanc(gencanc){
	this->gencanc->SetLabelText(_("Cancel"));
	this->gencanc->SetToolTip(_("Stop process!"));
}

Generator::~Generator(){
	// TODO: lock for progress
	this->gencanc->Enable(true);	// Disabled in case of cancel operation
	this->gencanc->SetLabelText(_("Generate"));
	this->gencanc->SetToolTip(_("Start process!"));
}

wxThread::ExitCode Generator::Entry(){
	// TODO: create temporary file & lua process

	// TODO: execute Lua scripts

	// TODO: on success, save temporary file

	// Success sound
	{
		wxSound file_sound(this->sound);
		if(file_sound.IsOk())
			file_sound.Play();
		else
			wxSound(wxT("nyu_sound"), true).Play();
	}
	// Execute after process command
	if(!this->command.IsEmpty())
		wxExecute(this->command);
	// Run ends
	return 0;
}