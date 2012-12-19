#include "GUI.h"
#include "System.h"
#include <wx/dir.h>
#include <wx/stdpaths.h>

GUI::GUI(const wxArrayString &args) : wxFrame(0, wxID_ANY, wxT("NyuFX"), wxDefaultPosition, wxDefaultSize,
											  wxSYSTEM_MENU | wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER | wxCLIP_CHILDREN){
	// Set window icon
	this->SetIcon(wxICON(exe_ico));
	//Set window background color
	this->SetBackgroundColour(wxColor(196,196,255));
	// Enable tooltips
	ConfigTooltips(1000, 5000, 0, 200);






	// Create menu
	this->CreateMenu();
}

void GUI::CreateMenu(){
	//Menu bar
	this->fileMenu = new wxMenu;
	this->fileMenu->AppendCheckItem(wxID_NONE, _("NONE"))->Enable(false);
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(ID_MENU_NEW, _("New\tCTRL+F1"), _("Clear file of focused editor"));
	this->fileMenu->Append(ID_MENU_OPEN, _("Open\tCTRL+F2"), _("Open file for focused editor"));
	this->fileMenu->Append(ID_MENU_RELOAD, _("Reload\tCTRL+R"), _("Reload opened file for focused editor"));
	this->fileMenu->Append(ID_MENU_SAVE, _("Save\tCTRL+S"), _("Save file of focused editor"));
	this->fileMenu->Append(ID_MENU_SAVE_AS, _("Save as...\tCTRL+F3"), _("Save file with specific target of focused editor"));
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(ID_MENU_QUIT, _("Quit\tALT+F4"), _("Close NyuFX"));

	this->editMenu = new wxMenu;
	this->editMenu->Append(ID_MENU_UNDO, _("Undo\tCTRL+Z"), _("Undo action"));
	this->editMenu->Append(ID_MENU_REDO, _("Redo\tCTRL+Y"), _("Redo action"));
	this->editMenu->AppendSeparator();
	this->editMenu->Append(ID_MENU_CUT, _("Cut\tCTRL+X"), _("Cut selection to clipboard"));
	this->editMenu->Append(ID_MENU_COPY, _("Copy\tCTRL+C"), _("Copy selection to clipboard"));
	this->editMenu->Append(ID_MENU_PASTE, _("Paste\tCTRL+V"), _("Paste clipboard"));
	this->editMenu->Append(ID_MENU_DELETE, _("Delete\tBACK"), _("Delete selection"));
	this->editMenu->Append(ID_MENU_SELECT_ALL, _("Select all\tCTRL+A"), _("Select all text in focus"));
	this->editMenu->AppendSeparator();
	this->editMenu->Append(ID_MENU_REPLACE, _("Replace\tCTRL+F"), _("Search and replace text"));
	this->editMenu->Append(ID_MENU_AUTO_STYLE, _("Auto-Style\tF1"), _("Style text"));

	this->viewMenu = new wxMenu;
	this->editviewMenu = new wxMenu;
	this->editviewMenu->AppendCheckItem(ID_MENU_VIEW_LUA, _("Lua editor\tSHIFT+F1"), _("Toggle view for Lua editor"))->Check(true);
	this->editviewMenu->Break();
	this->editviewMenu->AppendCheckItem(ID_MENU_VIEW_ASS, _("ASS editor\tSHIFT+F2"), _("Toggle view for ASS editor"))->Check(true);
	this->viewMenu->AppendSubMenu(editviewMenu, _("Editor view"), _("Toggle editors visibility"));
	this->viewMenu->AppendSeparator();
	this->viewMenu->Append(ID_MENU_GOTO, _("Go to line...\tCTRL+G"), _("Moves caret to line"));
	this->viewMenu->Append(ID_MENU_ZOOM_IN, _("Zoom in\tPGUP"), _("Zooms one level nearer to editor content"));
	this->viewMenu->Append(ID_MENU_ZOOM_OUT, _("Zoom out\tPGDN"), _("Zooms one level farther to editor content"));
	this->viewMenu->Append(ID_MENU_FOLD_ALL, _("Fold all\tSHIFT+F3"), _("Folds all lines in Lua editor"));
	this->viewMenu->Append(ID_MENU_UNFOLD_ALL, _("Unfold all\tSHIFT+F4"), _("Unfolds all lines in Lua editor"));
	this->viewMenu->AppendSeparator();
	this->openviewMenu = new wxMenu;
	this->openviewMenu->Append(ID_MENU_OPEN_INCL, _("Includes\tF2"), _("Open include folder"));
	this->openviewMenu->Append(ID_MENU_OPEN_TMPL, _("Templates\tF3"), _("Open templates folder"));
	this->openviewMenu->Append(ID_MENU_OPEN_TOOLS, _("Tools\tF4"), _("Open tools folder"));
	this->viewMenu->AppendSubMenu(openviewMenu, _("Folders"), _("Open folders of NyuFX"));

	this->resetMenu = new wxMenu;
	this->resetMenu->Append(ID_MENU_CLEAR_LOG, _("Log\tALT+F1"), _("Clear log text"));
	this->resetMenu->Append(ID_MENU_RESET_PROG, _("Progressbar\tALT+F2"), _("Set progressbar to 0%"));

	this->toolMenu = new wxMenu;
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\tools\\"), &tools, wxT("*.exe"), wxDIR_FILES);
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\tools\\"), &tools, wxT("*.lnk"), wxDIR_FILES);
	for(unsigned int i = 0; i<tools.GetCount() && i<30; i++){
		this->toolMenu->Append(ID_MENU_TOOL+i, tools[i].AfterLast('\\').BeforeLast('.'));
		// TODO: this->Connect(ID_MENU_TOOL+i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUI::OnTool));
	}

	this->helpMenu = new wxMenu;
	this->helpMenu->Append(ID_MENU_HELP, _("Help browser\tF10"), _("Help for NyuFX usage"));
	this->helpMenu->Append(ID_MENU_ABOUT, _("About\tF11"), _("Informations about NyuFX"));

	this->mbar = new wxMenuBar;
	this->mbar->Append(fileMenu, _("File"));
	this->mbar->Append(editMenu, _("Edit"));
	this->mbar->Append(viewMenu, _("View"));
	this->mbar->Append(resetMenu, _("Reset"));
	this->mbar->Append(toolMenu, _("Tools"));
	this->mbar->Append(helpMenu, _("Help"));
	this->SetMenuBar(mbar);

	//Status bar
	this->CreateStatusBar(1, wxSTB_DEFAULT_STYLE | wxBORDER_SUNKEN)->SetBackgroundColour(wxColour(164,164,255));
}
