#include "About.h"

About::About(wxWindow *wnd) : wxDialog(wnd, wxID_ANY, _("About"), wxDefaultPosition, wxDefaultSize, 0){
	// Window
	this->panel = new wxPanel(this, wxID_ANY);
	this->panel->SetBackgroundColour(wxColor(225,225,225));
	this->v_box = new wxBoxSizer(wxVERTICAL);
	// Logo
	wxBitmap bmp = wxBITMAP(logo_bmp);
	this->pic = new wxStaticBitmap(this->panel, wxID_ANY, bmp);
	this->v_box->Add(this->pic, 0, wxEXPAND | wxALL, 5);
	// Description
	this->text = new wxTextCtrl(this->panel, ID_URL_CLICK, wxEmptyString, wxDefaultPosition, wxSize(bmp.GetWidth(),220),
						wxTE_NO_VSCROLL | wxTE_LEFT | wxTE_BESTWRAP | wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2 | wxTE_AUTO_URL | wxSUNKEN_BORDER);
	wxTextAttr attrib;
	attrib.SetFontSize(10);
	attrib.SetFontWeight(wxFONTWEIGHT_BOLD);
	attrib.SetFontUnderlined(true);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(wxT("NyuFX v1.6.0\n"));
	attrib.SetFontUnderlined(false);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(_("Karaoke effect creation with pixel & vector utilities.\n\n"));
	this->text->AppendText(_("Programmer:\n"));
	attrib.SetFontWeight(wxFONTWEIGHT_NORMAL);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(wxT("  Youka\n"));
	attrib.SetFontWeight(wxFONTWEIGHT_BOLD);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(_("Thanks to:\n"));
	attrib.SetFontWeight(wxFONTWEIGHT_NORMAL);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(wxT("  wxWidgets - Copyright \u00A9 Robert Roebling & Co.\n"));
	this->text->AppendText(wxT("  Scintilla - Copyright \u00A9 Neil Hodgson\n"));
	this->text->AppendText(wxT("  Lua - Copyright \u00A9 PUC-Rio.\n"));
	attrib.SetFontWeight(wxFONTWEIGHT_BOLD);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(_("Website:\n"));
	attrib.SetFontWeight(wxFONTWEIGHT_NORMAL);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(wxT("  http://forum.youka.de\n"));
	attrib.SetFontWeight(wxFONTWEIGHT_BOLD);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(wxT("Copyright:\n"));
	attrib.SetFontWeight(wxFONTWEIGHT_NORMAL);
	this->text->SetDefaultStyle(attrib);
	this->text->AppendText(wxT("  www.gnu.org/copyleft/gpl.html"));
	this->text->SetInsertionPoint(0);
	this->v_box->Add(this->text, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
	// Button
	this->ok = new wxButton(panel, wxID_OK, _("Close"));
	this->ok->SetCursor(wxCURSOR_HAND);
	this->v_box->Add(this->ok, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
	// Place everything
	this->panel->SetSizer(this->v_box);
	this->v_box->SetSizeHints(this);
	// Focus about window
	this->Center();
	this->ok->SetFocus();
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(About, wxDialog)
	EVT_TEXT_URL(ID_URL_CLICK, About::OnURLClick)
END_EVENT_TABLE()

// Define event handlers
void About::OnURLClick(wxTextUrlEvent &url){
	if(url.GetMouseEvent().LeftDown())
		wxLaunchDefaultBrowser(this->text->GetRange(url.GetURLStart(), url.GetURLEnd()));
}