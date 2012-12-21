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
}

void GUI::SetMeta(){
	// Enable tooltips
	ConfigTooltips(1000, 5000, 0, 200);
	// Disable logging
	EnableLogging(false);
	// Load configuration settings
	Config::Load();
	// Set language to display
	wxString *language = Config::Language();
	if(*language == wxT("english"))
		SetLanguage(wxLANGUAGE_ENGLISH);
	else if(*language == wxT("german"))
		SetLanguage(wxLANGUAGE_GERMAN);
	else if(*language == wxT("japanese"))
		SetLanguage(wxLANGUAGE_JAPANESE);
	else if(*language == wxT("arabic"))
		SetLanguage(wxLANGUAGE_ARABIC);
}

void GUI::CreateMenu(){
	// Create file menu
	this->fileMenu = new wxMenu;
	this->fileMenu->AppendCheckItem(wxID_NONE, _("NONE"))->Enable(false);
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
	this->openviewMenu->Append(ID_MENU_OPEN_INCL, _("Includes") + wxT("\tF2"), _("Open include folder"));
	this->openviewMenu->Append(ID_MENU_OPEN_TMPL, _("Templates") + wxT("\tF3"), _("Open templates folder"));
	this->openviewMenu->Append(ID_MENU_OPEN_TOOLS, _("Tools") + wxT("\tF4"), _("Open tools folder"));
	this->viewMenu->AppendSubMenu(this->openviewMenu, _("Folders"), _("Open folders of NyuFX"));
	// Create reset menu
	this->resetMenu = new wxMenu;
	this->resetMenu->Append(ID_MENU_CLEAR_LOG, _("Log") + wxT("\tALT+F1"), _("Clear log text"));
	this->resetMenu->Append(ID_MENU_RESET_PROG, _("Progressbar") + wxT("\tALT+F2"), _("Set progressbar to 0%"));
	// Create tool menu
	this->toolMenu = new wxMenu;
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\tools\\"), &this->tools, wxT("*.exe"), wxDIR_FILES);
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\tools\\"), &this->tools, wxT("*.lnk"), wxDIR_FILES);
	for(unsigned int i = 0; i < this->tools.GetCount() && i < 30; i++){
		this->toolMenu->Append(ID_MENU_TOOL+i, this->tools[i].AfterLast('\\').BeforeLast('.'));
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
}
