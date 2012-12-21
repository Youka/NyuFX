#include "TaskIcon.h"

TaskIcon::TaskIcon(wxFrame *parent) : parent(parent){
	SetIcon(wxICON(exe_ico), wxT("NyuFX"));	// Set icon and name for taskbar
}

wxMenu* TaskIcon::CreatePopupMenu(){
	// Create menu, add items and give it to client
	wxMenu *popup = new wxMenu;
	popup->Append(wxID_CLOSE, _("Close"));
	return popup;
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(TaskIcon, wxTaskBarIcon)
	EVT_TASKBAR_LEFT_DCLICK(TaskIcon::OnTaskLDBClick)
	EVT_MENU(wxID_CLOSE, TaskIcon::OnClose)
END_EVENT_TABLE()

// Define event handlers
void TaskIcon::OnTaskLDBClick(wxTaskBarIconEvent& event){
	if(this->parent->IsIconized()){
		this->parent->Show();
		this->parent->Iconize(false);
	}else
		this->parent->Iconize(true);
}

void TaskIcon::OnClose(wxCommandEvent& event){
	this->parent->Close();
}
