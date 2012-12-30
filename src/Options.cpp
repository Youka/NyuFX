#include "Options.h"
#include "Config.h"

Options::Options(wxWindow *wnd) : wxDialog(wnd, wxID_ANY, _("Options"), wxDefaultPosition, wxDefaultSize,  wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX){
	// Window
	this->v_box = new wxBoxSizer(wxVERTICAL);
	this->flex = new wxFlexGridSizer(4, 2, 5, 15);
	// Language
	this->language_label = new wxStaticText(this, wxID_ANY, _("Language"));
	this->language_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->language_label->SetToolTip(_("GUI language"));
	this->flex->Add(this->language_label);
	const wxString supported_langs[] = {wxT("english"), wxT("german"), wxT("arabic"), wxT("japanese"), wxT("chinese")};
	this->languages = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, sizeof(supported_langs) / sizeof(wxString), supported_langs);
	if( this->languages->FindString(*Config::Language(), true) == wxNOT_FOUND )
		this->languages->SetValue(supported_langs[0]);
	else
		this->languages->SetValue(*Config::Language());
	this->flex->Add(this->languages);
	// Sound
	this->sound_label = new wxStaticText(this, wxID_ANY, _("Sound"));
	this->sound_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->sound_label->SetToolTip(_("Sound file to play after generation"));
	this->flex->Add(this->sound_label);
	this->sound_file = new wxTextCtrl(this, wxID_ANY, *Config::Sound(), wxDefaultPosition, wxDefaultSize,
								wxTE_NO_VSCROLL | wxTE_LEFT | wxTE_CHARWRAP | wxSUNKEN_BORDER);
	this->sound_file->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	this->sound_file->SetCursor(wxCURSOR_IBEAM);
	this->flex->Add(this->sound_file);
	// Minimize to icon?
	this->m2i_label = new wxStaticText(this, wxID_ANY, _("Taskicon?"));
	this->m2i_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->m2i_label->SetToolTip(_("Minimize program to taskicon? (otherwise to taskbar)"));
	this->flex->Add(this->m2i_label);
	this->minimize2icon = new wxCheckBox(this, wxID_ANY, wxEmptyString);
	this->minimize2icon->SetValue(*Config::Minimize2Icon());
	this->flex->Add(this->minimize2icon);
	// Font
	this->font_label = new wxStaticText(this, wxID_ANY, _("Font"));
	this->font_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
	this->font_label->SetToolTip(_("Default font of both editors"));
	this->flex->Add(this->font_label);
	// TODO: change font selection control (combobox & spinctrl)
	this->font = new wxFontPickerCtrl(this, wxID_ANY, wxFont( *Config::FontSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, *Config::Font() ));
	this->flex->Add(this->font);
	this->v_box->Add(this->flex, 0, wxLEFT | wxTOP | wxRIGHT, 8);
	// Separation line
	this->line = new wxStaticLine(this);
	this->v_box->Add(this->line, 0, wxEXPAND | wxTOP, 5);
	// Information
	this->info_label = new wxStaticText(this, wxID_ANY, _("Some changes require a program restart"));
	this->info_label->SetFont(wxFont(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	this->v_box->Add(this->info_label, 0, wxALIGN_CENTER | wxBOTTOM, 5);
	// Close button
	this->close = new wxButton(this, wxID_CLOSE, _("Close"), wxDefaultPosition, wxSize(-1,22));
	this->v_box->Add(this->close, 1, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 8);
	// Place everything
	this->SetSizer(this->v_box);
	this->v_box->SetSizeHints(this);
	// Focus window
	this->close->SetFocus();
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(Options, wxDialog)
	EVT_BUTTON(wxID_CLOSE, Options::OnButton)
	EVT_CLOSE(Options::OnClose)
END_EVENT_TABLE()

// Define event handlers
void Options::OnButton(wxCommandEvent& event){
	this->ProcessEvent( wxCloseEvent(wxEVT_CLOSE_WINDOW) );
}
void Options::OnClose(wxCloseEvent& event){
	// TODO: save settings
	this->Destroy();
}