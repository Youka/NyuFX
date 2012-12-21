#include "replace.h"

Replace::Replace(wxWindow *wnd, wxStyledTextCtrl *textctrl, wxMenuItem *item) : wxFindReplaceDialog(wnd, &this->finder_data, wxT(""), wxFR_REPLACEDIALOG) , editor(textctrl),item(item){
	// Disable actions on menu item while this dialog is shown
	this->item->Enable(false);
	// Decide for a dialog title, depending on editor target
	if(this->editor->GetProperty(wxT("fold")) == wxT("1"))
		this->SetTitle(_("Search and replace - Lua editor"));
	else
		this->SetTitle(_("Search and replace - ASS editor"));
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(Replace, wxFindReplaceDialog)
	EVT_FIND(wxID_ANY, Replace::OnFindAction)
	EVT_FIND_NEXT(wxID_ANY, Replace::OnFindAction)
	EVT_FIND_REPLACE(wxID_ANY, Replace::OnFindAction)
	EVT_FIND_REPLACE_ALL(wxID_ANY, Replace::OnFindAction)
	EVT_FIND_CLOSE(wxID_ANY, Replace::OnFindClose)
END_EVENT_TABLE()

// Define event handlers
void Replace::OnFindClose(wxFindDialogEvent& event){
	// Enable actions on menu item again
	this->item->Enable(true);
}

void Replace::OnFindAction(wxFindDialogEvent& event){
	int type = event.GetEventType();
	if (type == wxEVT_COMMAND_FIND_REPLACE_ALL){
		int cur_pos, end, hit;
		cur_pos = this->editor->GetCurrentPos();
		if( this->editor->GetSelectionStart() != this->editor->GetSelectionEnd() ){
			end = this->editor->GetSelectionEnd();
			this->editor->GotoPos( this->editor->GetSelectionStart() );
		}else{
			end = this->editor->GetTextLength();
			this->editor->DocumentStart();
		}
		this->editor->BeginUndoAction();
		do{
			hit = this->editor->FindText(this->editor->GetCurrentPos(), end, event.GetFindString(), event.GetFlags());
			if(hit >= 0){
				this->editor->SetSelection(hit,hit+event.GetFindString().Len());
				this->editor->ReplaceSelection(event.GetReplaceString());
				this->editor->GotoPos(this->editor->GetSelectionEnd());
				end += event.GetReplaceString().Len() - event.GetFindString().Len();
			}
		}while(hit >= 0);
		this->editor->EndUndoAction();
		this->editor->GotoPos(cur_pos);
	}else if(type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT){
		int hit = this->editor->FindText(this->editor->GetCurrentPos(), this->editor->GetTextLength(), event.GetFindString(), event.GetFlags());
		if(hit >= 0){
			this->editor->GotoPos(hit);
			this->editor->SetSelection(hit,hit+event.GetFindString().Len());
		}else
			this->editor->DocumentStart();
	}else if(type == wxEVT_COMMAND_FIND_REPLACE && this->editor->GetSelectionStart() != this->editor->GetSelectionEnd()){
		this->editor->ReplaceSelection(event.GetReplaceString());
		this->editor->SetSelection(this->editor->GetCurrentPos()-event.GetReplaceString().Len(),this->editor->GetCurrentPos());
	}
}