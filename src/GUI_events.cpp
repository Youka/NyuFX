#include "GUI.h"
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include "Config.h"
#include "HelpWindow.h"
#include "About.h"

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(GUI, wxFrame)
	// Meta events
	EVT_MENU_OPEN(GUI::OnMenuOpen)
	EVT_ICONIZE(GUI::OnIconize)
	EVT_CLOSE(GUI::OnClose)
	// File menu
	EVT_MENU(ID_MENU_NEW, GUI::OnNew)
	EVT_MENU(ID_MENU_OPEN, GUI::OnOpen)
	EVT_MENU(ID_MENU_RELOAD, GUI::OnReload)
	EVT_MENU(ID_MENU_SAVE, GUI::OnSave)
	EVT_MENU(ID_MENU_SAVE_AS, GUI::OnSaveAs)
	EVT_MENU(ID_MENU_QUIT, GUI::OnQuit)
	// Edit menu
	EVT_MENU(ID_MENU_UNDO, GUI::OnUndo)
	EVT_MENU(ID_MENU_REDO, GUI::OnRedo)
	EVT_MENU(ID_MENU_CUT, GUI::OnCut)
	EVT_MENU(ID_MENU_COPY, GUI::OnCopy)
	EVT_MENU(ID_MENU_PASTE, GUI::OnPaste)
	EVT_MENU(ID_MENU_DELETE, GUI::OnDelete)
	EVT_MENU(ID_MENU_SELECT_ALL, GUI::OnSelectAll)
	EVT_MENU(ID_MENU_REPLACE, GUI::OnReplace)
	EVT_MENU(ID_MENU_AUTO_STYLE, GUI::OnAutoStyle)
	// View menu
	EVT_MENU(ID_MENU_VIEW_LUA, GUI::OnViewLua)
	EVT_MENU(ID_MENU_VIEW_ASS, GUI::OnViewASS)
	EVT_MENU(ID_MENU_GOTO, GUI::OnGotoLine)
	EVT_MENU(ID_MENU_ZOOM_IN, GUI::OnZoomIn)
	EVT_MENU(ID_MENU_ZOOM_OUT, GUI::OnZoomOut)
	EVT_MENU(ID_MENU_FOLD_ALL, GUI::OnFoldAll)
	EVT_MENU(ID_MENU_UNFOLD_ALL, GUI::OnUnfoldAll)
	EVT_MENU(ID_MENU_OPEN_INCL, GUI::OnOpenInclude)
	EVT_MENU(ID_MENU_OPEN_TMPL, GUI::OnOpenTemplates)
	EVT_MENU(ID_MENU_OPEN_TOOLS, GUI::OnOpenTools)
	EVT_MENU(ID_MENU_OPEN_OPTIONS, GUI::OnOpenOptions)
	// Reset menu
	EVT_MENU(ID_MENU_CLEAR_LOG, GUI::OnClearLog)
	EVT_MENU(ID_MENU_RESET_PROG, GUI::OnResetProgressbar)

	// OnTool defined during menu creation

	// Help menu
	EVT_MENU(ID_MENU_HELP, GUI::OnHelp)
	EVT_MENU(ID_MENU_ABOUT, GUI::OnAbout)
END_EVENT_TABLE()

// Define event handlers
void GUI::OnMenuOpen(wxMenuEvent& event){
	// Filemenu opened
	if(event.GetMenu() == this->fileMenu)
		// Lua focused
		if(this->lua_editor->editor->HasFocus()){
			this->fileMenu->FindItemByPosition(0)->Check(true);
			this->fileMenu->FindItemByPosition(0)->SetItemLabel(_("Lua editor"));
			this->fileMenu->FindItemByPosition(2)->Enable(true);
			this->fileMenu->FindItemByPosition(3)->Enable(true);
			this->fileMenu->FindItemByPosition(4)->Enable(true);
			this->fileMenu->FindItemByPosition(5)->Enable(true);
			this->fileMenu->FindItemByPosition(6)->Enable(true);
		// ASS focused
		}else if(this->ass_editor->editor->HasFocus()){
			this->fileMenu->FindItemByPosition(0)->Check(true);
			this->fileMenu->FindItemByPosition(0)->SetItemLabel(_("ASS editor"));
			this->fileMenu->FindItemByPosition(2)->Enable(true);
			this->fileMenu->FindItemByPosition(3)->Enable(true);
			this->fileMenu->FindItemByPosition(4)->Enable(true);
			this->fileMenu->FindItemByPosition(5)->Enable(true);
			this->fileMenu->FindItemByPosition(6)->Enable(true);
		// Anything else focused
		}else{
			this->fileMenu->FindItemByPosition(0)->Check(false);
			this->fileMenu->FindItemByPosition(0)->SetItemLabel(_("NONE"));
			this->fileMenu->FindItemByPosition(2)->Enable(false);
			this->fileMenu->FindItemByPosition(3)->Enable(false);
			this->fileMenu->FindItemByPosition(4)->Enable(false);
			this->fileMenu->FindItemByPosition(5)->Enable(false);
			this->fileMenu->FindItemByPosition(6)->Enable(false);
		}
	// Editmenu opened
	else if(event.GetMenu() == this->editMenu)
		// Lua or ASS focused
		if(this->lua_editor->editor->HasFocus() || this->ass_editor->editor->HasFocus()){
			this->editMenu->FindItemByPosition(9)->Enable(true);
			this->editMenu->FindItemByPosition(10)->Enable(true);
		// Anything else focused
		}else{
			this->editMenu->FindItemByPosition(9)->Enable(false);
			this->editMenu->FindItemByPosition(10)->Enable(false);
		}
	// Viewmenu opened
	else if(event.GetMenu() == this->viewMenu)
		// Lua focused
		if(this->lua_editor->editor->HasFocus()){
			this->viewMenu->FindItemByPosition(2)->Enable(true);
			this->viewMenu->FindItemByPosition(3)->Enable(true);
			this->viewMenu->FindItemByPosition(4)->Enable(true);
			this->viewMenu->FindItemByPosition(5)->Enable(true);
			this->viewMenu->FindItemByPosition(6)->Enable(true);
		// ASS focused
		}else if(this->ass_editor->editor->HasFocus()){
			this->viewMenu->FindItemByPosition(2)->Enable(true);
			this->viewMenu->FindItemByPosition(3)->Enable(true);
			this->viewMenu->FindItemByPosition(4)->Enable(true);
			this->viewMenu->FindItemByPosition(5)->Enable(false);
			this->viewMenu->FindItemByPosition(6)->Enable(false);
		// Anything else focused
		}else{
			this->viewMenu->FindItemByPosition(2)->Enable(false);
			this->viewMenu->FindItemByPosition(3)->Enable(false);
			this->viewMenu->FindItemByPosition(4)->Enable(false);
			this->viewMenu->FindItemByPosition(5)->Enable(false);
			this->viewMenu->FindItemByPosition(6)->Enable(false);
		}
}
void GUI::OnIconize(wxIconizeEvent& event){
	// Taskbar or taskicon
	if(event.IsIconized() && this->taskicon)
		this->Hide();
}
void GUI::OnClose(wxCloseEvent &event){
	// Abort running generation?
	if(this->output_panel->gencanc->GetLabelText() != _("Generate") && wxMessageBox(_("Do you really want to abort process?"), _("Abort"), wxYES_NO | wxCENTRE | wxICON_WARNING) != wxYES)
		return;
	// Save config & close program
	Config::Save();
	this->Destroy();
}

void GUI::OnNew(wxCommandEvent& event){

}
void GUI::OnOpen(wxCommandEvent& event){

}
void GUI::OnReload(wxCommandEvent& event){

}
void GUI::OnSave(wxCommandEvent& event){

}
void GUI::OnSaveAs(wxCommandEvent& event){

}
void GUI::OnQuit(wxCommandEvent& event){
	this->Close();
}

void GUI::OnUndo(wxCommandEvent& event){

}
void GUI::OnRedo(wxCommandEvent& event){

}
void GUI::OnCut(wxCommandEvent& event){

}
void GUI::OnCopy(wxCommandEvent& event){

}
void GUI::OnPaste(wxCommandEvent& event){

}
void GUI::OnDelete(wxCommandEvent& event){

}
void GUI::OnSelectAll(wxCommandEvent& event){

}
void GUI::OnReplace(wxCommandEvent& event){

}
void GUI::OnAutoStyle(wxCommandEvent& event){

}

void GUI::OnViewLua(wxCommandEvent& event){

}
void GUI::OnViewASS(wxCommandEvent& event){

}
void GUI::OnGotoLine(wxCommandEvent& event){

}
void GUI::OnZoomIn(wxCommandEvent& event){

}
void GUI::OnZoomOut(wxCommandEvent& event){

}
void GUI::OnFoldAll(wxCommandEvent& event){

}
void GUI::OnUnfoldAll(wxCommandEvent& event){

}
void GUI::OnOpenInclude(wxCommandEvent& event){
	wxLaunchDefaultApplication( wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\include\\") );
}
void GUI::OnOpenTemplates(wxCommandEvent& event){
	wxLaunchDefaultApplication( wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\templates\\") );
}
void GUI::OnOpenTools(wxCommandEvent& event){
	wxLaunchDefaultApplication( wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\tools\\") );
}
void GUI::OnOpenOptions(wxCommandEvent& event){

}

void GUI::OnClearLog(wxCommandEvent& event){

}
void GUI::OnResetProgressbar(wxCommandEvent& event){

}

void GUI::OnTool(wxCommandEvent& event){
	wxString tool = this->tools[event.GetId()-ID_MENU_TOOL];
	if(tool.AfterLast('.') == "exe")
		wxExecute( tool );
	else
		wxLaunchDefaultApplication( tool );
}

void GUI::OnHelp(wxCommandEvent& event){
	static HelpWindow *help = new HelpWindow(this);
	help->Show();
}
void GUI::OnAbout(wxCommandEvent& event){
	this->SetTransparent(127);
	About(this).ShowModal();
	this->SetTransparent(255);
}