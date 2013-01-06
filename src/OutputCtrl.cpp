#include "OutputCtrl.h"

// File drop handler
#include <wx/dnd.h>

class DropOutputFile : public wxFileDropTarget{
	public:
		// Store reference to drop target
		DropOutputFile(wxTextCtrl *filetarget) : file(filetarget){}
		// Handle file drop
		virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames){
			this->file->SetValue(filenames[0]);
			this->file->SetInsertionPointEnd();
			return true;
		}
	private:
		// Drop target
		wxTextCtrl *file;
};

//Output panel
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include "Generator.h"
#include "Config.h"

OutputCtrl::OutputCtrl(wxWindow *wnd) : wxPanel(wnd){
	this->SetBackgroundColour( this->GetParent()->GetBackgroundColour() );
	// Create subwindows
	this->CreateElements();
	// Place window elements
	this->PlaceElements();
}

void OutputCtrl::CreateElements(){
	//Logo
	int width = wxBITMAP(logo_bmp).GetWidth();
	this->logo = new wxStaticBitmap(this, wxID_ANY, wxBITMAP(logo_bmp));
	//Log
	this->log_text = new wxStaticText(this, wxID_ANY, _("Log"));
	this->log_text->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->log = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(width,200),
								wxTE_LEFT | wxHSCROLL | wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);
	this->log->SetBackgroundColour(wxColor(225,225,225));
	this->log->SetToolTip(_("Output by 'print' function"));
	//Progress bar
	this->progress_text = new wxStaticText(this, wxID_ANY, _("Progress"));
	this->progress_text->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->progressbar = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(width,25));
	this->progressbar->SetToolTip(_("Set by 'io.progressbar' function"));
	//Output controls
	this->input_text = new wxStaticText(this, wxID_ANY, _("Output"));
	this->input_text->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->input_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(width,110), wxRAISED_BORDER);
	this->input_panel->SetBackgroundColour(wxColor(255,255,255));
	this->input_panel->SetToolTip(_("Output options"));
	//Inputs
	this->out_file = new wxTextCtrl(this->input_panel, wxID_ANY, wxEmptyString, wxPoint(5,5), wxSize(width-95,25),
							  wxTE_NO_VSCROLL | wxTE_LEFT | wxTE_CHARWRAP | wxSUNKEN_BORDER);
	this->out_file->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	this->out_file->SetBackgroundColour(wxColor(240,240,240));
	this->out_file->SetDropTarget(new DropOutputFile(out_file));
	this->choose = new wxButton(this->input_panel, ID_OUT_CHOOSE_FILE, _("Choose"), wxPoint(width-90,5), wxSize(80,25));
	this->choose->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	this->choose->SetCursor(wxCURSOR_HAND);
	this->choose->SetToolTip(_("Choose an output file"));
	this->gencanc = new wxButton(this->input_panel, ID_OUT_GENCANC, _("Generate"), wxPoint(5,35), wxSize(80,25));
	this->gencanc->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->gencanc->SetCursor(wxCURSOR_HAND);
	this->gencanc->SetToolTip(_("Start process!"));
	this->open = new wxButton(this->input_panel, ID_OUT_OPEN, _("Open"), wxPoint(90,35), wxSize(80,25));
	this->open->SetCursor(wxCURSOR_HAND);
	this->open->SetToolTip(_("Open output file"));
	this->after_proc = new wxStaticText(this->input_panel, wxID_ANY, _("After process command"), wxPoint(8,65));
	this->after_proc->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	this->cmd = new wxTextCtrl(this->input_panel, wxID_ANY, wxEmptyString, wxPoint(5,80), wxSize(width-15,20),
						 wxTE_NO_VSCROLL | wxTE_LEFT | wxTE_CHARWRAP | wxDOUBLE_BORDER);
	this->cmd->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	this->cmd->SetBackgroundColour(wxColor(0,0,0));
	this->cmd->SetForegroundColour(wxColor(0,196,0));
}

void OutputCtrl::PlaceElements(){
	this->v_box = new wxBoxSizer(wxVERTICAL);

	this->v_box->AddSpacer(5);
	this->v_box->Add(this->logo);
	this->v_box->Add(this->log_text);
	this->v_box->Add(this->log, 1, wxGROW);
	this->v_box->AddSpacer(5);
	this->v_box->Add(this->progress_text);
	this->v_box->Add(this->progressbar, 0, wxGROW);
	this->v_box->AddSpacer(5);
	this->v_box->Add(this->input_text);
	this->v_box->Add(this->input_panel, 0, wxGROW);
	this->v_box->AddSpacer(5);

	this->SetSizer(this->v_box);
	this->v_box->SetSizeHints(this);
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(OutputCtrl, wxPanel)
	EVT_BUTTON(ID_OUT_CHOOSE_FILE, OutputCtrl::OnChooseFile)
	EVT_BUTTON(ID_OUT_GENCANC, OutputCtrl::OnGenerateCancel)
	EVT_BUTTON(ID_OUT_OPEN, OutputCtrl::OnOpen)
END_EVENT_TABLE()

// Define event handlers
void OutputCtrl::OnChooseFile(wxCommandEvent& event){
	wxFileDialog input(this->GetParent(), _("Choose output file"), wxEmptyString, wxEmptyString, wxT("Advanced substation alpha (*.ass)|*.ass"),
					   wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if(input.ShowModal() == wxID_OK){
		this->out_file->SetValue(input.GetPath());
		this->out_file->SetInsertionPointEnd();
	}
}

void OutputCtrl::OnGenerateCancel(wxCommandEvent& event){
	// Process thread handle
	static Generator *gen_thread;
	// Start new process
	if(this->gencanc->GetLabelText() == _("Generate")){
		gen_thread = new Generator(
			reinterpret_cast<wxTextCtrl*>(this->GetParent()->FindWindow(wxT("splitter"))->GetChildren()[0]->FindWindow(wxTextCtrlNameStr))->GetValue(),	// Lua editor title
			reinterpret_cast<wxTextCtrl*>(this->GetParent()->FindWindow(wxT("splitter"))->GetChildren()[1]->FindWindow(wxTextCtrlNameStr))->GetValue(),	// ASS editor title
			this->out_file->GetValue(),
			this->cmd->GetValue(),
			*Config::Sound(),
			this->log,
			this->progressbar,
			this->gencanc
		);
		if(gen_thread->Create() != wxTHREAD_NO_ERROR){
			gen_thread->Delete();
			wxMessageBox(_("Couldn't create thread!"), _("Process error"), wxOK | wxCENTRE | wxICON_ERROR);
		}else if(gen_thread->Run() != wxTHREAD_NO_ERROR){
			gen_thread->Delete();
			wxMessageBox(_("Couldn't run thread!"), _("Process error"), wxOK | wxCENTRE | wxICON_ERROR);
		}
	// Cancel current process
	}else if(this->gencanc->GetLabelText() == _("Cancel")){
		this->gencanc->Enable(false);
		this->gencanc->SetLabelText(wxT("..."));
		this->gencanc->SetToolTip(_("Currently terminating process!"));
		gen_thread->Delete();
	}
}

void OutputCtrl::OnOpen(wxCommandEvent& event){
	if(!this->out_file->GetValue().IsEmpty() && !wxLaunchDefaultApplication(this->out_file->GetValue()))
		wxMessageBox(_("File not exists!"), _("Output file error"), wxOK | wxCENTRE | wxICON_ERROR);
}