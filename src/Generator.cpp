#include "Generator.h"
#include <wx/sound.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include "Scripting.h"

Generator::Generator(wxString lua_file, wxString ass_file, wxString output_file, wxString command, wxString sound, wxTextCtrl *log, wxGauge *progressbar, wxButton *gencanc)
: lua_file(lua_file), ass_file(ass_file), output_file(output_file), command(command), sound(sound), log(log), progressbar(progressbar), gencanc(gencanc){
	this->gencanc->SetLabelText(_("Cancel"));
	this->gencanc->SetToolTip(_("Stop process!"));
}

Generator::~Generator(){
	if(!wxThread::IsMain())	// Queued in destroy list in main thread (or deleted after run in current thread)?
		wxMutexGuiEnter();
	this->gencanc->Enable(true);	// Disabled in case of cancel operation
	this->gencanc->SetLabelText(_("Generate"));
	this->gencanc->SetToolTip(_("Start process!"));
	if(!wxThread::IsMain())
		wxMutexGuiLeave();
}

wxThread::ExitCode Generator::Entry(){
	// Create Lua environment process
	Scripting env(this->log, this->progressbar);
	// Load & execute Lua scripts
	wxArrayString inc_files;
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\include\\"), &inc_files, wxT("*.lua"), wxDIR_FILES);
	for(unsigned int i = 0; i < inc_files.GetCount(); i++)
		if( !env.DoFile(inc_files[i]) )
			return 0;
	if( !env.CallInit(2, this->ass_file, this->output_file) )
		return 0;
	if( !env.DoFile(this->lua_file) )
		return 0;
	if( !env.CallExit() )
		return 0;
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