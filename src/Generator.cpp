#include "Generator.h"
#include <wx/sound.h>

Generator::Generator(wxString lua_file, wxString ass_file, wxString output_file, wxString command, wxString sound, wxTextCtrl *log, wxGauge *progressbar, wxButton *gencanc)
: lua_file(lua_file), ass_file(ass_file), output_file(output_file), command(command), sound(sound), log(log), progressbar(progressbar), gencanc(gencanc){
	this->gencanc->SetLabelText(_("Cancel"));
	this->gencanc->SetToolTip(_("Stop process!"));
}

Generator::~Generator(){
	wxMutexGuiLocker gui_locker;
	this->gencanc->Enable(true);	// Disabled in case of cancel operation
	this->gencanc->SetLabelText(_("Generate"));
	this->gencanc->SetToolTip(_("Start process!"));
}

wxThread::ExitCode Generator::Entry(){
	// TODO: create Lua process & register filenames + functions

	// TODO: execute Lua scripts

	// Success sound
	if(this->sound.Trim().IsEmpty())
		wxSound(wxT("nyu_sound"), true).Play();
	else
		wxSound::Play(this->sound);
	// Execute after process command
	if(!this->command.IsEmpty())
		wxExecute(this->command);
	// Run ends
	return 0;
}