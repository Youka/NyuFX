#include "Config.h"
#include <wx/app.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>

// Configuration attributes
wxString in_lua, in_ass, out_ass, cmd, language, sound, font;
bool auto_run = false;
unsigned long fontsize = 11;
bool minimize2icon = false;

namespace Config{
	wxString* LuaInput(){
		return &in_lua;
	}
	wxString* ASSInput(){
		return &in_ass;
	}
	wxString* ASSOutput(){
		return &out_ass;
	}
	wxString* Command(){
		return &cmd;
	}
	bool AutoRun(){
		return auto_run;
	}
	wxString* Language(){
		return &language;
	}
	wxString* Sound(){
		return &sound;
	}
	wxString* Font(){
		return &font;
	}
	unsigned long* FontSize(){
		return &fontsize;
	}
	bool* Minimize2Icon(){
		return &minimize2icon;
	}

	void Load(){
		// Evaluate config file
		wxFileInputStream stream(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + "\\config.ini");
		if(stream.IsOk()){
			wxFileConfig conf(stream, wxMBConvUTF8());
			conf.SetPath(wxT("/Input"));
			conf.Read(wxT("lua"), &in_lua);
			conf.Read(wxT("ass"), &in_ass);
			conf.SetPath(wxT("/Output"));
			conf.Read(wxT("ass"), &out_ass);
			conf.Read(wxT("cmd"), &cmd);
			conf.SetPath(wxT("/Interface"));
			conf.Read(wxT("language"), &language);
			conf.Read(wxT("sound"), &sound);
			conf.Read(wxT("font"), &font);
			conf.Read(wxT("minimize2icon"), &minimize2icon);
			wxString value;
			if(conf.Read(wxT("fontsize"), &value))
				value.ToULong(&fontsize);
		}
		// Evaluate command line
		enum : unsigned char{INPUT = 0, OUTPUT, COMMAND} status = INPUT;
		const wxArrayString args = wxAppConsole::GetInstance()->argv.GetArguments();
		wxString arg;
		for(int i = 1; i < args.GetCount(); i++){	// First argument is the application name, so skip it
			arg = args[i];
			if(arg == wxT("-i"))
				status = INPUT;
			else if(arg == wxT("-o"))
				status = OUTPUT;
			else if(arg == wxT("-cmd"))
				status = COMMAND;
			else if(arg == wxT("-run")){
				status = INPUT;
				auto_run = true;
			}else{
				switch(status){
					case INPUT:
						if(arg.AfterLast('.') == wxT("lua"))
							in_lua = arg;
						else if(arg.AfterLast('.') == wxT("ass"))
							in_ass = arg;
						break;
					case OUTPUT:
						out_ass = arg;
						break;
					case COMMAND:
						cmd = arg;
						break;
				}
				status = INPUT;
			}
		}
	}

	void Save(){
		wxFileConfig conf(wxEmptyString,	// application name
										wxEmptyString,	// vendor name
										wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + "\\config.ini",	// local file
										wxEmptyString,	// global file
										wxCONFIG_USE_LOCAL_FILE,	// content target
										wxMBConvUTF8());	// format
		conf.SetPath(wxT("/Input"));
		conf.Write(wxT("lua"), in_lua);
		conf.Write(wxT("ass"), in_ass);
		conf.SetPath(wxT("/Output"));
		conf.Write(wxT("ass"), out_ass);
		conf.Write(wxT("cmd"), cmd);
		conf.SetPath(wxT("/Interface"));
		conf.Write(wxT("language"), language);
		conf.Write(wxT("sound"), sound);
		conf.Write(wxT("minimize2icon"), minimize2icon);
		conf.Write(wxT("font"), font);
		conf.Write(wxT("fontsize"), fontsize);
	}
}