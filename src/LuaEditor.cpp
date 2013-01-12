#include "LuaEditor.h"

LuaEditor::LuaEditor(wxWindow *wnd) : EditorBase(wnd){
	// Title
	this->title->SetToolTip(_("Lua editor"));
	this->title->SetBackgroundColour(wxColor(0, 0, 255));
	this->check->BackgroundColor(wxColor(0, 0, 255));
	// Style
	this->LoadStyle();
	// Bind event handlers for folding & brace highlighting
	this->Connect(wxID_ANY, wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(LuaEditor::OnMarginClick));
	this->Connect(wxID_ANY, wxEVT_STC_UPDATEUI, wxStyledTextEventHandler(LuaEditor::OnBrace));
}

void LuaEditor::LoadStyle(){
	// Lua style
	EditorBase::SetDefaultStyle();
	this->editor->SetLexer(wxSTC_LEX_LUA);
	this->editor->StyleSetForeground(wxSTC_LUA_DEFAULT,	   wxColour(0,0,0));
	this->editor->StyleSetForeground(wxSTC_LUA_COMMENT,	   wxColour(128,128,128));
	this->editor->StyleSetItalic(wxSTC_LUA_COMMENT,		   true);
	this->editor->StyleSetForeground(wxSTC_LUA_COMMENTLINE,   wxColour(128,128,128));
	this->editor->StyleSetItalic(wxSTC_LUA_COMMENTLINE,	   true);
	this->editor->StyleSetForeground(wxSTC_LUA_COMMENTDOC,	wxColour(128,128,128));
	this->editor->StyleSetItalic(wxSTC_LUA_COMMENTDOC,		true);
	this->editor->StyleSetForeground(wxSTC_LUA_NUMBER,		wxColour(0,196,0));
	this->editor->StyleSetForeground(wxSTC_LUA_STRING,		wxColour(225,0,0));
	this->editor->StyleSetForeground(wxSTC_LUA_CHARACTER,	 wxColour(225,128,0));
	this->editor->StyleSetForeground(wxSTC_LUA_LITERALSTRING, wxColour(196,0,0));
	this->editor->StyleSetForeground(wxSTC_LUA_PREPROCESSOR,  wxColour(64,64,64));
	this->editor->StyleSetForeground(wxSTC_LUA_OPERATOR,	  wxColour(128,0,196));
	this->editor->StyleSetBold(wxSTC_LUA_OPERATOR, true);
	this->editor->StyleSetForeground(wxSTC_LUA_IDENTIFIER,	wxColour(0,0,0));
	this->editor->StyleSetForeground(wxSTC_LUA_STRINGEOL,	 wxColour(225,0,0));
	this->editor->StyleSetForeground (wxSTC_LUA_WORD,		 wxColour(0,0,255));
	this->editor->StyleSetBold(wxSTC_LUA_WORD, true);
	this->editor->StyleSetForeground (wxSTC_LUA_WORD2,		wxColour(0,0,255));
	this->editor->StyleSetForeground (wxSTC_LUA_WORD3,		wxColour(0,128,255));
	this->editor->StyleSetForeground (wxSTC_LUA_WORD4,		wxColour(196,196,0));
	// Key words
	this->editor->SetKeyWords(0, wxT("function for while repeat until if else elseif end break return in do then"));
	this->editor->SetKeyWords(1, wxT("local"));
	this->editor->SetKeyWords(2, wxT("and or not"));
	this->editor->SetKeyWords(3, wxT("nil true false"));
	// Brace matching
	this->editor->StyleSetForeground(wxSTC_STYLE_BRACELIGHT, wxColour(255,255,255));
	this->editor->StyleSetBackground(wxSTC_STYLE_BRACELIGHT, wxColour(128,0,196));
	this->editor->StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColour(255,255,255));
	this->editor->StyleSetBackground(wxSTC_STYLE_BRACEBAD, wxColour(255,0,0));
	// Margin (folding)
	this->editor->SetMarginType(MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);
	this->editor->SetMarginWidth(MARGIN_FOLD, 15);
	this->editor->SetMarginMask(MARGIN_FOLD, wxSTC_MASK_FOLDERS);
	this->editor->SetMarginSensitive(MARGIN_FOLD, true);
	// Folding
	this->editor->SetProperty(wxT("fold"),		 wxT("1") );
	this->editor->SetProperty(wxT("fold.comment"), wxT("1") );
	this->editor->SetProperty(wxT("fold.compact"), wxT("1") );
	this->editor->SetProperty(wxT("fold.preprocessor"), wxT("1") );
	this->editor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_PLUS, wxColor(225,0,0), wxColor(0,0,0));
	this->editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_MINUS, wxColor(0,0,225), wxColor(0,0,0));
	this->editor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
	this->editor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_EMPTY);
	this->editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_EMPTY);
	this->editor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
	this->editor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
}

void LuaEditor::OnMarginClick(wxStyledTextEvent& event){
	if (event.GetMargin() == MARGIN_FOLD){
		int line_clicked = this->editor->LineFromPosition(event.GetPosition());
		if ((this->editor->GetFoldLevel(line_clicked) & wxSTC_FOLDLEVELHEADERFLAG) > 0)
			this->editor->ToggleFold(line_clicked);
	}
}

void LuaEditor::OnBrace(wxStyledTextEvent& event){
	// Caret position
	int pos = this->editor->GetCurrentPos();
	// Keys at position
	char pre_key = static_cast<char>(this->editor->GetCharAt( pos-1 ));
	char key = static_cast<char>(this->editor->GetCharAt( pos ));
	// Brace search
	int hit = wxSTC_INVALID_POSITION;
	if( pre_key == '(' || pre_key == '{' || pre_key == '[' )
		hit = pos - 1;
	else if( key == ')' || key == '}' || key == ']')
		hit = pos;
	// Brace action
	if(hit != wxSTC_INVALID_POSITION){
		int match = this->editor->BraceMatch( hit );
		if(match != wxSTC_INVALID_POSITION)
			this->editor->BraceHighlight(match, hit);
		else
			this->editor->BraceBadLight(hit);
	}else
		this->editor->BraceBadLight(wxSTC_INVALID_POSITION);		// Remove any brace highlight
}