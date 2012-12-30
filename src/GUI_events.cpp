#include "GUI.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/numdlg.h>
#include <wx/stdpaths.h>
#include <wx/clipbrd.h>
#include <wx/regex.h>
#include "Config.h"
#include "HelpWindow.h"
#include "About.h"
#include "Replace.h"
#include "Options.h"

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
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->Clear();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->Clear();
}
void GUI::OnOpen(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus()){
		wxFileDialog input(this, _("Open Lua file"), wxEmptyString, wxEmptyString, wxT("Lua (*.lua)|*.lua"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if(input.ShowModal() == wxID_OK)
			this->lua_editor->LoadFile(input.GetPath());
	}else if(this->ass_editor->editor->HasFocus()){
		wxFileDialog input(this, _("Open ASS file"), wxEmptyString, wxEmptyString, wxT("Advanced substation alpha (*.ass)|*.ass"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if(input.ShowModal() == wxID_OK)
			this->ass_editor->LoadFile(input.GetPath());
	}
}
void GUI::OnReload(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->LoadFile(this->lua_editor->title->GetValue());
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->LoadFile(this->ass_editor->title->GetValue());
}
void GUI::OnSave(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->SaveFile(this->lua_editor->title->GetValue());
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->SaveFile(this->ass_editor->title->GetValue());
}
void GUI::OnSaveAs(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus()){
		wxFileDialog input(this, _("Save Lua file"), wxEmptyString, wxEmptyString, wxT("Lua (*.lua)|*.lua"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if(input.ShowModal() == wxID_OK)
			this->lua_editor->SaveFile(input.GetPath());
	}else if(this->ass_editor->editor->HasFocus()){
		wxFileDialog input(this, _("Save ASS file"), wxEmptyString, wxEmptyString, wxT("Advanced substation alpha (*.ass)|*.ass"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if(input.ShowModal() == wxID_OK)
			this->ass_editor->SaveFile(input.GetPath());
	}
}
void GUI::OnQuit(wxCommandEvent& event){
	this->Close();
}

void GUI::OnUndo(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->editor->Undo();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->editor->Undo();
	else if(this->output_panel->out_file->HasFocus())
		this->output_panel->out_file->Undo();
	else if(this->output_panel->cmd->HasFocus())
		this->output_panel->cmd->Undo();
}
void GUI::OnRedo(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->editor->Redo();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->editor->Redo();
	else if(this->output_panel->out_file->HasFocus())
		this->output_panel->out_file->Redo();
	else if(this->output_panel->cmd->HasFocus())
		this->output_panel->cmd->Redo();
}
void GUI::OnCut(wxCommandEvent& event){
	bool change = true;
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->editor->Cut();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->editor->Cut();
	else if(this->output_panel->out_file->HasFocus())
		this->output_panel->out_file->Cut();
	else if(this->output_panel->cmd->HasFocus())
		this->output_panel->cmd->Cut();
	else
		change = false;
	// Anything changed, so flush data to global clipboard
	if(change && wxTheClipboard->Open()){
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
	}
}
void GUI::OnCopy(wxCommandEvent& event){
	bool change = true;
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->editor->Copy();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->editor->Copy();
	else if(this->output_panel->out_file->HasFocus())
		this->output_panel->out_file->Copy();
	else if(this->output_panel->cmd->HasFocus())
		this->output_panel->cmd->Copy();
	else
		change = false;
	// Anything changed, so flush data to global clipboard
	if(change && wxTheClipboard->Open()){
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
	}
}
void GUI::OnPaste(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->editor->Paste();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->editor->Paste();
	else if(this->output_panel->out_file->HasFocus())
		this->output_panel->out_file->Paste();
	else if(this->output_panel->cmd->HasFocus())
		this->output_panel->cmd->Paste();
}
void GUI::OnDelete(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->editor->DeleteBack();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->editor->DeleteBack();
	else if(this->output_panel->out_file->HasFocus()){
		long from, to;
		this->output_panel->out_file->GetSelection(&from,&to);
		if(from == to && from != 0)
			this->output_panel->out_file->Remove(from-1,from);
		else if(from != to)
			this->output_panel->out_file->Remove(from,to);
	}else if(this->output_panel->cmd->HasFocus()){
		long from, to;
		this->output_panel->cmd->GetSelection(&from,&to);
		if(from == to && from != 0)
			this->output_panel->cmd->Remove(from-1,from);
		else if(from != to)
			this->output_panel->cmd->Remove(from,to);
	}
}
void GUI::OnSelectAll(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		this->lua_editor->editor->SelectAll();
	else if(this->ass_editor->editor->HasFocus())
		this->ass_editor->editor->SelectAll();
	else if(this->output_panel->log->HasFocus())
		this->output_panel->log->SelectAll();
	else if(this->output_panel->out_file->HasFocus())
		this->output_panel->out_file->SelectAll();
	else if(this->output_panel->cmd->HasFocus())
		this->output_panel->cmd->SelectAll();
}
void GUI::OnReplace(wxCommandEvent& event){
	// One instance each running
	static Replace *replace = 0;
	if(replace)
		replace->Destroy();
	static wxFindReplaceData data;
	// Create dialog
	if(this->lua_editor->editor->HasFocus()){
		replace = new Replace(this, &data, this->lua_editor->editor, this->editMenu->FindItem(ID_MENU_REPLACE));
		replace->Show();
	}else if(this->ass_editor->editor->HasFocus()){
		replace = new Replace(this, &data, this->ass_editor->editor, this->editMenu->FindItem(ID_MENU_REPLACE));
		replace->Show();
	}
}
void GUI::OnAutoStyle(wxCommandEvent& event){
	// Lua editor
	if(this->lua_editor->editor->HasFocus()){
		// Get text
		int cur_pos = this->lua_editor->editor->GetCurrentPos();
		wxString text = this->lua_editor->editor->GetText();
		// Clean text
		wxRegEx expr;
		// 1. Clear spaces
		expr.Compile(wxT("[ \t]+")); // No wide spaces
		expr.Replace(&text, wxT(" "));
		text.Replace(wxT("\r"), wxEmptyString); // No senseless carriage returns
		text.Replace(wxT("\n "), wxT("\n")); // No prespace
		expr.Compile(wxT("^ "));
		expr.Replace(&text, wxEmptyString);
		text.Replace(wxT(" \n"), wxT("\n")); // No postspace
		expr.Compile(wxT(" $"));
		expr.Replace(&text, wxEmptyString);
		expr.Compile(wxT("\n{3,}")); // Not more than 2 newlines in row
		expr.Replace(&text, wxT("\n\n"));
		// 2. Extract lines
		wxArrayString lines;
		int hit;
		while(true){
			hit = text.First(wxT("\n"));
			if(hit == wxNOT_FOUND){
				lines.Add(text);
				text.Clear();
				break;
			}else{
				lines.Add(text.Left(hit));
				text = text.Mid(hit+1);
			}
		}
		// 3. Indent lines & build text
		wxString indent, line;
		size_t n = lines.GetCount();
		for(unsigned int i=0; i < n; i++){
			line = lines[i];
			if(!indent.IsEmpty() && (
				line.Left(5) == wxT("until") ||
				line.Left(4) == wxT("else") ||
				line.Left(6) == wxT("elseif") ||
				line.Left(3) == wxT("end")
			   ))
				indent.RemoveLast();
			text.Append(indent + line);
			if(i+1 != n)
				text.Append(wxT("\n"));
			if(line.Left(8) == wxT("function") ||
			   line.Left(14) == wxT("local function") ||
				line.Left(3) == wxT("for") ||
				line.Left(5) == wxT("while") ||
				line.Left(6) == wxT("repeat") ||
				line.Left(2) == wxT("if") ||
				line.Left(4) == wxT("else") ||
				line.Left(6) == wxT("elseif") ||
				line.Right(4) == wxT("then") ||
				line.Right(2) == wxT("do")
			   )
				indent += wxT("\t");
		}
		// Return text
		this->lua_editor->editor->SetText(text);
		this->lua_editor->editor->GotoPos(cur_pos);
	// ASS editor
	}else if(this->ass_editor->editor->HasFocus()){
		// Get text
		int cur_pos = this->ass_editor->editor->GetCurrentPos();
		wxString text = this->ass_editor->editor->GetText();
		// Clean text
		wxRegEx expr;
		text.Replace(wxT("\r"), wxEmptyString); // No senseless carriage returns
		expr.Compile(wxT("\n[ \t]+"));  // No prespace
		expr.Replace(&text, wxT("\n"));
		expr.Compile(wxT("^[ \t]+"));
		expr.Replace(&text, wxEmptyString);
		expr.Compile(wxT("[ \t]+\n"));  // No postspace
		expr.Replace(&text, wxT("\n"));
		expr.Compile(wxT("[ \t]+$"));
		expr.Replace(&text, wxEmptyString);
		expr.Compile(wxT("\n{3,}")); // Not more than 2 newlines in row
		expr.Replace(&text, wxT("\n\n"));
		// Return text
		this->ass_editor->editor->SetText(text);
		this->ass_editor->editor->GotoPos(cur_pos);
	}
}

void GUI::OnViewLua(wxCommandEvent& event){
	if(this->splitter->IsSplit())
		this->splitter->Unsplit(this->lua_editor);
	else if(this->splitter->GetWindow1() == this->ass_editor)
		this->splitter->SplitVertically(this->lua_editor, this->ass_editor);
	else
		this->editviewMenu->Check(ID_MENU_VIEW_LUA, true);
}
void GUI::OnViewASS(wxCommandEvent& event){
	if(this->splitter->IsSplit())
		this->splitter->Unsplit(this->ass_editor);
	else if(this->splitter->GetWindow1() == this->lua_editor)
		this->splitter->SplitVertically(this->lua_editor, this->ass_editor);
	else
		this->editviewMenu->Check(ID_MENU_VIEW_ASS, true);
}
void GUI::OnGotoLine(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus()){
		long num = wxGetNumberFromUser(_("Choose line number"), wxEmptyString, _("Go to Lua line..."), 1, 1, this->lua_editor->editor->GetLineCount(), this);
		if(num > 0)
			this->lua_editor->editor->GotoLine( num-1 );
	}else if(this->ass_editor->editor->HasFocus()){
		long num = wxGetNumberFromUser(_("Choose line number"), wxEmptyString, _("Go to ASS line..."), 1, 1, this->ass_editor->editor->GetLineCount(), this);
		if(num > 0)
			this->ass_editor->editor->GotoLine( num-1 );
	}
}
void GUI::OnZoomIn(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus()){
		int zoom = this->lua_editor->editor->GetZoom();
		if(zoom < 20)
			this->lua_editor->editor->SetZoom(zoom+1);
	}else if(this->ass_editor->editor->HasFocus()){
		int zoom = this->ass_editor->editor->GetZoom();
		if(zoom < 20)
			this->ass_editor->editor->SetZoom(zoom+1);
	}
}
void GUI::OnZoomOut(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus()){
		int zoom = this->lua_editor->editor->GetZoom();
		if(zoom > -10)
			this->lua_editor->editor->SetZoom(zoom-1);
	}else if(this->ass_editor->editor->HasFocus()){
		int zoom = this->ass_editor->editor->GetZoom();
		if(zoom > -10)
			this->ass_editor->editor->SetZoom(zoom-1);
	}
}
void GUI::OnFoldAll(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		for(int i = 0; i < this->lua_editor->editor->GetLineCount(); i++)
			if((this->lua_editor->editor->GetFoldLevel(i) & wxSTC_FOLDLEVELHEADERFLAG) && this->lua_editor->editor->GetFoldExpanded(i))
				this->lua_editor->editor->ToggleFold(i);
}
void GUI::OnUnfoldAll(wxCommandEvent& event){
	if(this->lua_editor->editor->HasFocus())
		for(int i = 0; i < this->lua_editor->editor->GetLineCount(); i++)
			if((this->lua_editor->editor->GetFoldLevel(i) & wxSTC_FOLDLEVELHEADERFLAG) && !this->lua_editor->editor->GetFoldExpanded(i))
				this->lua_editor->editor->ToggleFold(i);
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
	if(Options(this).ShowModal() == 0)
		wxMessageBox(_("Attributes changed which require a program restart!"), _("Options"), wxOK | wxICON_WARNING);
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