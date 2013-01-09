#include "Options.h"
#include "Config.h"
#include "GUI.h"

#define GET_GUI reinterpret_cast<GUI*>(this->GetParent())

Options::Options(wxWindow *wnd) : wxDialog(wnd, wxID_ANY, _("Options"), wxDefaultPosition, wxDefaultSize,  wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX){
	// Create subwindows
	this->CreateElements();
	// Set layout
	this->PlaceElements();
}

void Options::CreateElements(){
	// Language
	this->language_label = new wxStaticText(this, wxID_ANY, _("Language"));
	this->language_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	this->language_label->SetToolTip(_("GUI language"));
	const wxString supported_langs[] = {wxT("english"), wxT("german"), wxT("arabic"), wxT("japanese"), wxT("chinese")};
	this->languages = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, sizeof(supported_langs) / sizeof(wxString), supported_langs, wxCB_READONLY);
	if( this->languages->FindString(*Config::Language(), true) == wxNOT_FOUND )
		this->languages->SetValue(supported_langs[0]);
	else
		this->languages->SetValue(*Config::Language());
	this->languages->SetCursor(wxCURSOR_HAND);
	// Sound
	this->sound_label = new wxStaticText(this, wxID_ANY, _("Sound"));
	this->sound_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	this->sound_label->SetToolTip(_("Sound file to play after generation"));
	this->sound_file = new wxFilePickerCtrl(this, wxID_ANY, *Config::Sound(), _("Choose sound file"), wxT("Waveform audio file (*.wav)|*.wav"));
	this->sound_file->GetTextCtrl()->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	this->sound_file->GetPickerCtrl()->SetLabelText(_("Choose"));
	this->sound_file->GetPickerCtrl()->SetCursor(wxCURSOR_HAND);
	// Minimize to icon?
	this->m2i_label = new wxStaticText(this, wxID_ANY, _("Taskicon?"));
	this->m2i_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	this->m2i_label->SetToolTip(_("Minimize program to taskicon? (otherwise to taskbar)"));
	this->minimize2icon = new wxCheckBox(this, wxID_ANY, wxEmptyString);
	this->minimize2icon->SetValue(*Config::Minimize2Icon());
	// Font
	this->font_label = new wxStaticText(this, wxID_ANY, _("Font"));
	this->font_label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	this->font_label->SetToolTip(_("Default font of both editors"));
	this->fontface = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1), wxFontEnumerator::GetFacenames(), wxCB_DROPDOWN | wxCB_SORT);
	if( this->fontface->FindString(*Config::Font(), true) == wxNOT_FOUND )
		this->fontface->SetValue(GET_GUI->lua_editor->editor->StyleGetFaceName(wxSTC_STYLE_DEFAULT));
	else
		this->fontface->SetValue(*Config::Font());
	this->fontface->SetCursor(wxCURSOR_HAND);
	this->fontsize = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 4, 50, *Config::FontSize());
	// Separation line
	this->line = new wxStaticLine(this);
	// Information
	this->info_label = new wxStaticText(this, wxID_ANY, _("Language change requires a program restart"));
	this->info_label->SetFont(wxFont(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
	// Close button
	this->accept = new wxButton(this, wxID_CLOSE, _("Accept"), wxDefaultPosition, wxSize(-1,22));
	this->accept->SetCursor(wxCURSOR_HAND);
}

void Options::PlaceElements(){
	this->v_box = new wxBoxSizer(wxVERTICAL);
	this->font_box = new wxBoxSizer(wxHORIZONTAL);
	this->flex = new wxFlexGridSizer(4, 2, 5, 15);

	this->flex->Add(this->language_label);
	this->flex->Add(this->languages);
	this->flex->Add(this->sound_label);
	this->flex->Add(this->sound_file, 0, wxEXPAND);
	this->flex->Add(this->m2i_label);
	this->flex->Add(this->minimize2icon);
	this->flex->Add(this->font_label);
	this->font_box->Add(this->fontface);
	this->font_box->Add(this->fontsize);
	this->flex->Add(this->font_box);
	this->v_box->Add(this->flex, 0, wxLEFT | wxTOP | wxRIGHT, 8);
	this->v_box->Add(this->line, 0, wxEXPAND | wxTOP, 5);
	this->v_box->Add(this->info_label, 0, wxALIGN_CENTER | wxBOTTOM, 5);
	this->v_box->Add(this->accept, 1, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 8);

	// Place everything
	this->SetSizer(this->v_box);
	this->v_box->SetSizeHints(this);
	// Focus window
	this->accept->SetFocus();
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(Options, wxDialog)
	EVT_BUTTON(wxID_CLOSE, Options::OnButton)
END_EVENT_TABLE()

// Define event handlers
void Options::OnButton(wxCommandEvent& event){
	// Save settings, ...
	*Config::Language() = this->languages->GetValue();
	*Config::Sound() = this->sound_file->GetTextCtrl()->GetValue();
	*Config::Minimize2Icon() = this->minimize2icon->IsChecked();
	*Config::Font() = this->fontface->GetValue();
	*Config::FontSize() = this->fontsize->GetValue();
	// ...update taskicon immediatly and...
	if(GET_GUI->taskicon == 0 && this->minimize2icon->GetValue() == true)
		GET_GUI->taskicon = new TaskIcon(GET_GUI);
	else if(GET_GUI->taskicon != 0 && this->minimize2icon->GetValue() == false){
		GET_GUI->taskicon->Destroy();
		GET_GUI->taskicon = 0;
	}
	GET_GUI->lua_editor->editor->StyleSetSize(wxSTC_STYLE_DEFAULT, this->fontsize->GetValue());
	GET_GUI->lua_editor->editor->StyleSetFaceName(wxSTC_STYLE_DEFAULT, this->fontface->GetValue());
	GET_GUI->lua_editor->LoadStyle();
	GET_GUI->ass_editor->editor->StyleSetSize(wxSTC_STYLE_DEFAULT, this->fontsize->GetValue());
	GET_GUI->ass_editor->editor->StyleSetFaceName(wxSTC_STYLE_DEFAULT, this->fontface->GetValue());
	GET_GUI->ass_editor->LoadStyle();
	// ...destroy dialog
	this->ProcessEvent( wxCloseEvent(wxEVT_CLOSE_WINDOW) );
}