#include "HelpWindow.h"

#include <wx/dir.h>
#include <wx/stdpaths.h>

//Frame
HelpWindow::HelpWindow(wxWindow *wnd) : wxFrame(wnd, wxID_ANY, _("Help"), wxDefaultPosition, wxDefaultSize,
										  wxSYSTEM_MENU | wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER | wxCLIP_CHILDREN){
	// Set window icon
	this->SetIcon(wxICON(help_ico));
	// Create subwindows
	this->CreateElements();
	// Place window elements
	this->PlaceElements();
	// Fill content list
	this->SetContent();
}

void HelpWindow::CreateElements(){
	// Create content list
	this->entries = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(150,-1), 0, reinterpret_cast<wxString*>(NULL), wxLB_HSCROLL);
	this->entries->SetBackgroundColour(wxColor(225, 225, 225));
	this->entries->SetCursor(wxCURSOR_RIGHT_ARROW);
	this->entries->SetToolTip(_("HTML documents in 'docs' folder"));
	// Create browser title
	this->title = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
								wxTE_NO_VSCROLL | wxTE_LEFT | wxTE_CHARWRAP | wxTE_RICH | wxTE_PROCESS_ENTER | wxDOUBLE_BORDER);
	this->title->SetCursor(wxCURSOR_IBEAM);
	this->title->SetToolTip(_("Current URL"));
	this->title->Connect(wxID_ANY, wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(HelpWindow::OnEnter));
	// Create title buttons
	wxSize button_size = wxSize(this->title->GetSize().y, this->title->GetSize().y);
	this->enter = new wxButton(this, wxID_ANY, wxT("!"), wxDefaultPosition, button_size);
	this->enter->SetCursor(wxCURSOR_HAND);
	this->enter->SetToolTip(_("Load page"));
	this->stop = new wxButton(this, wxID_ANY, wxT("X"), wxDefaultPosition, button_size);
	this->stop->SetCursor(wxCURSOR_HAND);
	this->stop->SetToolTip(_("Stop page loading"));
	this->back = new wxButton(this, wxID_ANY, wxT("<"), wxDefaultPosition, button_size);
	this->back->Enable(false);
	this->back->SetCursor(wxCURSOR_HAND);
	this->back->SetToolTip(_("Back in history"));
	this->forward = new wxButton(this, wxID_ANY, wxT(">"), wxDefaultPosition, button_size);
	this->forward->Enable(false);
	this->forward->SetCursor(wxCURSOR_HAND);
	this->forward->SetToolTip(_("Forward in history"));
	// Create browser field
	this->web = wxWebView::New(this, wxID_ANY, wxT("www.google.de"), wxDefaultPosition, wxSize(350, 500), wxWEB_VIEW_BACKEND_DEFAULT, wxSUNKEN_BORDER);
}

void HelpWindow::PlaceElements(){
	this->main_hbox = new wxBoxSizer(wxHORIZONTAL);
	this->title_hbox = new wxBoxSizer(wxHORIZONTAL);
	this->web_vbox = new wxBoxSizer(wxVERTICAL);

	this->main_hbox->Add(this->entries, 0, wxGROW);
	this->title_hbox->Add(this->title, 1, wxGROW);
	this->title_hbox->Add(this->enter);
	this->title_hbox->Add(this->stop);
	this->title_hbox->Add(this->back);
	this->title_hbox->Add(this->forward);
	this->web_vbox->Add(this->title_hbox, 0, wxGROW);
	this->web_vbox->Add(this->web, 1, wxGROW);
	this->main_hbox->Add(this->web_vbox, 1, wxGROW);

	this->SetSizer(this->main_hbox);
	this->main_hbox->SetSizeHints(this);

	this->Center();
	this->Maximize(true);
}

void HelpWindow::SetContent(){
	wxDir::GetAllFiles(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\docs\\"), &this->files, wxT("*.html"), wxDIR_FILES);
	for(unsigned int i = 0; i < this->files.GetCount(); i++)
		this->entries->Append( this->files[i].BeforeLast('.').AfterLast('\\') );
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(HelpWindow, wxFrame)
	EVT_CLOSE(HelpWindow::OnClose)
	EVT_LISTBOX(wxID_ANY, HelpWindow::OnSelect)
	EVT_BUTTON(wxID_ANY, HelpWindow::OnHistoryAction)
	EVT_WEB_VIEW_LOADED(wxID_ANY, HelpWindow::OnLoaded)
END_EVENT_TABLE()

// Define event handlers
void HelpWindow::OnClose(wxCloseEvent &event){
	Destroy();
}

void HelpWindow::CheckHistory(){
	this->title->SetValue( this->web->GetCurrentURL() );
	if(this->web->CanGoForward())
		this->forward->Enable(true);
	else
		this->forward->Enable(false);
	if(this->web->CanGoBack())
		this->back->Enable(true);
	else
		this->back->Enable(false);
}

void HelpWindow::OnSelect(wxCommandEvent& event){
	this->web->ClearHistory();
	this->back->Enable(false);
	this->forward->Enable(false);
	int pos = this->entries->GetSelection();
	if(wxFileExists(this->files[pos])){
		this->web->LoadURL(this->files[pos]);
		this->CheckHistory();
	}
}

void HelpWindow::OnEnter(wxCommandEvent& event){
	wxCommandEvent entered(wxEVT_COMMAND_BUTTON_CLICKED, wxID_ANY);
	GetEventHandler()->ProcessEvent( entered );
}

void HelpWindow::OnHistoryAction(wxCommandEvent& event){
	wxObject *obj = event.GetEventObject();
	if(obj == this->back)
		this->web->GoBack();
	else if(obj == this->forward)
		this->web->GoForward();
	else if(obj == this->stop)
		this->web->Stop();
	else
		this->web->LoadURL(this->title->GetValue());
	this->CheckHistory();
}

void HelpWindow::OnLoaded(wxWebViewEvent& event){
	this->CheckHistory();
}