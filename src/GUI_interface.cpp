#include "GUI.h"
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include "System.h"
#include "Config.h"

GUI::GUI() : wxFrame(0, wxID_ANY, wxT("NyuFX"), wxDefaultPosition, wxDefaultSize,
											  wxSYSTEM_MENU | wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER | wxCLIP_CHILDREN){
	// Define global settings
	this->SetMeta();
	// Set window icon
	this->SetIcon(wxICON(exe_ico));
	//Set window background color
	this->SetBackgroundColour(wxColor(196,196,255));
	// Create menu
	this->CreateMenu();
	// Create subwindows
	this->CreateElements();
	// Set layout
	this->PlaceElements();
	// Load config settings to interface
	this->ReadConfig();
}

GUI::~GUI(){
	// Remove system tray
	if(this->taskicon)
		this->taskicon->Destroy();
	// Save current configuration settings
	*Config::LuaInput() = this->lua_editor->title->GetValue();
	*Config::ASSInput() = this->ass_editor->title->GetValue();
	*Config::ASSOutput() = this->output_panel->out_file->GetValue();
	*Config::Command() = this->output_panel->cmd->GetValue();
	Config::Save();
}

void GUI::SetMeta(){
	// Load configuration settings
	Config::Load();
	// Set language to display
	wxString *language = Config::Language();
	if(*language == wxT("german"))
		SetLanguage(wxLANGUAGE_GERMAN);
	else if(*language == wxT("japanese"))
		SetLanguage(wxLANGUAGE_JAPANESE);
	else if(*language == wxT("arabic"))
		SetLanguage(wxLANGUAGE_ARABIC);
	else if(*language == wxT("chinese"))
		SetLanguage(wxLANGUAGE_CHINESE);
	else
		SetLanguage(wxLANGUAGE_ENGLISH);
	// Enable tooltips
	ConfigTooltips(1000, 5000, 0, 200);
	// Disable logging
	EnableLogging(false);
}

void GUI::CreateMenu(){
	// Create file menu
	this->fileMenu = new wxMenu;
	this->fileMenu->AppendCheckItem(ID_MENU_ACTIVE, _("NONE"), _("Active editor"))->Enable(false);
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(ID_MENU_NEW, _("New") + wxT("\tCTRL+F1"), _("Clear file of focused editor"));
	this->fileMenu->Append(ID_MENU_OPEN, _("Open") + wxT("\tCTRL+F2"), _("Open file for focused editor"));
	this->fileMenu->Append(ID_MENU_RELOAD, _("Reload") + wxT("\tCTRL+R"), _("Reload opened file for focused editor"));
	this->fileMenu->Append(ID_MENU_SAVE, _("Save") + wxT("\tCTRL+S"), _("Save file of focused editor"));
	this->fileMenu->Append(ID_MENU_SAVE_AS, _("Save as...") + wxT("\tCTRL+F3"), _("Save file with specific target of focused editor"));
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(ID_MENU_QUIT, _("Quit") + wxT("\tALT+F4"), _("Close NyuFX"));
	// Create edit menu
	this->editMenu = new wxMenu;
	this->editMenu->Append(ID_MENU_UNDO, _("Undo") + wxT("\tCTRL+Z"), _("Undo action"));
	this->editMenu->Append(ID_MENU_REDO, _("Redo") + wxT("\tCTRL+Y"), _("Redo action"));
	this->editMenu->AppendSeparator();
	this->editMenu->Append(ID_MENU_CUT, _("Cut") + wxT("\tCTRL+X"), _("Cut selection to clipboard"));
	this->editMenu->Append(ID_MENU_COPY, _("Copy") + wxT("\tCTRL+C"), _("Copy selection to clipboard"));
	this->editMenu->Append(ID_MENU_PASTE, _("Paste") + wxT("\tCTRL+V"), _("Paste clipboard"));
	this->editMenu->Append(ID_MENU_DELETE, _("Delete") + wxT("\tBACK"), _("Delete selection"));
	this->editMenu->Append(ID_MENU_SELECT_ALL, _("Select all") + wxT("\tCTRL+A"), _("Select all text in focus"));
	this->editMenu->AppendSeparator();
	this->editMenu->Append(ID_MENU_REPLACE, _("Replace") + wxT("\tCTRL+F"), _("Search and replace text"));
	this->editMenu->Append(ID_MENU_AUTO_STYLE, _("Auto-Style") + wxT("\tF1"), _("Style text"));
	// Create view menu
	this->viewMenu = new wxMenu;
	this->editviewMenu = new wxMenu;
	this->editviewMenu->AppendCheckItem(ID_MENU_VIEW_LUA, _("Lua editor") + wxT("\tSHIFT+F1"), _("Toggle view for Lua editor"))->Check(true);
	this->editviewMenu->Break();
	this->editviewMenu->AppendCheckItem(ID_MENU_VIEW_ASS, _("ASS editor") + wxT("\tSHIFT+F2"), _("Toggle view for ASS editor"))->Check(true);
	this->viewMenu->AppendSubMenu(this->editviewMenu, _("Editor view"), _("Toggle editors visibility"));
	this->viewMenu->AppendSeparator();
	this->viewMenu->Append(ID_MENU_GOTO, _("Go to line...") + wxT("\tCTRL+G"), _("Moves caret to line"));
	this->viewMenu->Append(ID_MENU_ZOOM_IN, _("Zoom in") + wxT("\tPGUP"), _("Zooms one level nearer to editor content"));
	this->viewMenu->Append(ID_MENU_ZOOM_OUT, _("Zoom out") + wxT("\tPGDN"), _("Zooms one level farther to editor content"));
	this->viewMenu->Append(ID_MENU_FOLD_ALL, _("Fold all") + wxT("\tSHIFT+F3"), _("Folds all lines in Lua editor"));
	this->viewMenu->Append(ID_MENU_UNFOLD_ALL, _("Unfold all") + wxT("\tSHIFT+F4"), _("Unfolds all lines in Lua editor"));
	this->viewMenu->AppendSeparator();
	this->openviewMenu = new wxMenu;
	this->openviewMenu->Append(ID_MENU_OPEN_INCL, wxT("Include\tF2"), _("Open include folder"));
	this->openviewMenu->Append(ID_MENU_OPEN_TMPL, wxT("Templates\tF3"), _("Open templates folder"));
	this->openviewMenu->Append(ID_MENU_OPEN_TOOLS, wxT("Tools\tF4"), _("Open tools folder"));
	this->viewMenu->AppendSubMenu(this->openviewMenu, _("Folders"), _("Open folders of NyuFX"));
	this->viewMenu->AppendSeparator();
	this->viewMenu->Append(ID_MENU_OPEN_OPTIONS, _("Options") + wxT("\tCTRL+O"), _("Application options"));
	// Create reset menu
	this->resetMenu = new wxMenu;
	this->resetMenu->Append(ID_MENU_CLEAR_LOG, _("Log") + wxT("\tALT+F1"), _("Clear log text"));
	this->resetMenu->Append(ID_MENU_RESET_PROG, _("Progressbar") + wxT("\tALT+F2"), _("Set progressbar to 0%"));
	// Create tool menu
	this->toolMenu = new wxMenu;
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\tools\\"), &this->tools, wxT("*.exe"), wxDIR_FILES);
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\tools\\"), &this->tools, wxT("*.lnk"), wxDIR_FILES);
	for(unsigned int i = 0; i < this->tools.GetCount() && i < 30; i++){
		this->toolMenu->Append(ID_MENU_TOOL+i, this->tools[i].AfterLast('\\').BeforeLast('.'), this->tools[i]);
		this->Connect(ID_MENU_TOOL+i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUI::OnTool));
	}
	// Create help menu
	this->helpMenu = new wxMenu;
	this->helpMenu->Append(ID_MENU_HELP, _("Help browser") + wxT("\tF10"), _("Help for NyuFX usage"));
	this->helpMenu->Append(ID_MENU_ABOUT, _("About") + wxT("\tF11"), _("Informations about NyuFX"));
	// Create menu bar with existing menus
	wxMenuBar *mbar = new wxMenuBar;
	mbar->Append(this->fileMenu, _("File"));
	mbar->Append(this->editMenu, _("Edit"));
	mbar->Append(this->viewMenu, _("View"));
	mbar->Append(this->resetMenu, _("Reset"));
	mbar->Append(this->toolMenu, _("Tools"));
	mbar->Append(this->helpMenu, _("Help"));
	this->SetMenuBar(mbar);
	// Create status bar
	this->CreateStatusBar(1, wxSTB_DEFAULT_STYLE | wxBORDER_SUNKEN)->SetBackgroundColour(wxColour(164,164,255));
	// Create system tray
	this->taskicon = *Config::Minimize2Icon() ? new TaskIcon(this) : 0;
}

void GUI::CreateElements(){
	// Splitter window
	this->splitter = new wxSplitterWindow(this, wxID_ANY);
	this->splitter->SetMinimumPaneSize(10);
	this->splitter->SetSashGravity(0.5);
	// Lua editor
	this->lua_editor = new LuaEditor(this->splitter);
	// ASS editor
	this->ass_editor = new ASSEditor(this->splitter);
	// Output panel
	this->output_panel = new OutputCtrl(this);
}

void GUI::PlaceElements(){
	// Set layout
	this->h_box = new wxBoxSizer(wxHORIZONTAL);

	this->splitter->SplitVertically(this->lua_editor, this->ass_editor);
	this->h_box->Add(this->splitter, 1, wxGROW);
	this->h_box->AddSpacer(5);
	this->h_box->Add(this->output_panel, 0, wxEXPAND | wxALIGN_RIGHT);
	this->h_box->AddSpacer(5);

	this->SetSizer(this->h_box);
	this->h_box->SetSizeHints(this);

	// Frame position
	this->Center();
	this->Maximize(true);
}

void GUI::ReadConfig(){
	// Set editor files
	this->lua_editor->LoadFile(*Config::LuaInput());
	this->ass_editor->LoadFile(*Config::ASSInput());
	// Set output fields
	this->output_panel->out_file->SetValue(*Config::ASSOutput());
	this->output_panel->out_file->SetInsertionPointEnd();
	this->output_panel->cmd->SetValue(*Config::Command());
	this->output_panel->cmd->SetInsertionPointEnd();
	// Run immediately?
	if(Config::AutoRun())
		this->output_panel->GetEventHandler()->ProcessEvent( wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, this->output_panel->gencanc->GetId()) );
}