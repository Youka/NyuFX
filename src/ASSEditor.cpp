#include "ASSEditor.h"
#include <wx/ffile.h>

ASSEditor::ASSEditor(wxWindow *wnd) : EditorBase(wnd){
	// Title
	this->title->SetToolTip(_("ASS editor"));
	this->title->SetBackgroundColour(wxColor(255, 0, 0));
	this->check->BackgroundColor(wxColor(255, 0, 0));
	// Style
	this->LoadStyle();
}

void ASSEditor::LoadStyle(){
   // INI style
	EditorBase::SetDefaultStyle();
	this->editor->SetLexer(wxSTC_LEX_PROPERTIES);
	this->editor->StyleSetForeground(wxSTC_PROPS_COMMENT, wxColour(128,128,128));
	this->editor->StyleSetItalic(wxSTC_PROPS_COMMENT,true);
	this->editor->StyleSetForeground(wxSTC_PROPS_SECTION, wxColour(0,0,255));
	this->editor->StyleSetBold(wxSTC_PROPS_SECTION, true);
	this->editor->StyleSetForeground(wxSTC_PROPS_ASSIGNMENT, wxColour(255,0,0));
	this->editor->StyleSetForeground(wxSTC_PROPS_DEFVAL, wxColour(255,0,0));
	this->editor->StyleSetForeground(wxSTC_PROPS_KEY, wxColour(255,0,0));
}