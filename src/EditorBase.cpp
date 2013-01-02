#include "EditorBase.h"

// File drop handler
#include <wx/dnd.h>
#include <wx/ffile.h>

class DropEditorFile : public wxFileDropTarget{
	public:
		// Store reference to drop targets
		DropEditorFile(wxTextCtrl *title, wxStyledTextCtrl *editor) : title(title), editor(editor){}
		// Handle file drop
		virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames){
			wxString filename = filenames[0];
			wxFFile file;
			if(file.Open(filename, "r")){
				wxString content;
				file.ReadAll(&content);
				this->editor->SetText(content);
				this->editor->DocumentStart();
				this->editor->EmptyUndoBuffer();
				this->title->SetValue(filename);
			}
			return true;
		}
	private:
		// Drop targets
		wxTextCtrl *title;
		wxStyledTextCtrl *editor;
};

// Editor
#include "Config.h"

EditorBase::EditorBase(wxWindow* wnd) : wxPanel(wnd, wxID_ANY){
	this->CreateElements();
	this->PlaceElements();
	this->ConfigureEditor();
}

void EditorBase::CreateElements(){
	// Save symbol
	this->check = new SaveSymbol(this, wxPoint(0,0), wxSize(18,18));
	// Opened file
	this-> title = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
						   wxTE_NO_VSCROLL | wxTE_LEFT | wxTE_CHARWRAP | wxTE_READONLY | wxTE_RICH | wxNO_BORDER);
	this->title->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	this->title->SetCursor(wxCURSOR_IBEAM);
	// Editor
	this->editor = new wxStyledTextCtrl(this, wxID_ANY);
	if(!Config::Font()->IsEmpty()){
		this->editor->StyleSetSize(wxSTC_STYLE_DEFAULT, *Config::FontSize());
		this->editor->StyleSetFaceName(wxSTC_STYLE_DEFAULT, *Config::Font());
	}
	// Drop target
	this->editor->SetDropTarget(new DropEditorFile(this->title, this->editor));
}

void EditorBase::PlaceElements(){
	this->v_box = new wxBoxSizer(wxVERTICAL);
	this->h_box = new wxBoxSizer(wxHORIZONTAL);

	this->h_box->Add(this->check);
	this->h_box->Add(this->title, 1, wxGROW);
	this->v_box->Add(this->h_box, 0, wxGROW);
	this->v_box->Add(this->editor, 1, wxGROW);

	this->SetSizer(this->v_box);
	this->v_box->SetSizeHints(this);
}

void EditorBase::ConfigureEditor(){
	// Empty text
	this->editor->SetText(wxEmptyString);
	// Disable popup menu
	this->editor->UsePopUp(false);
	// No text wrapping
	this->editor->SetWrapMode(wxSTC_WRAP_NONE);
	// Windows end-of-line
	this->editor->SetEOLMode(wxSTC_EOL_CRLF);
	// Caret visibility
	this->editor->SetVisiblePolicy(wxSTC_VISIBLE_STRICT|wxSTC_VISIBLE_SLOP, 1);
	this->editor->SetXCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
	this->editor->SetYCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
	// Allow tabulators with 8 spaces
	this->editor->SetTabWidth(8);
	this->editor->SetUseTabs(true);
	// Allow indentation
	this->editor->SetTabIndents(true);
	this->editor->SetIndentationGuides(1);
	// Enable line numbers
	this->editor->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
	this->editor->SetMarginWidth(MARGIN_LINE_NUMBERS, 30);
	// Bind event handlers for save symbol updates & auto-indention of the caret in new lines
	this->Connect(wxID_ANY, wxEVT_STC_MODIFIED, wxStyledTextEventHandler(EditorBase::OnModify));
	this->Connect(wxID_ANY, wxEVT_STC_CHARADDED, wxStyledTextEventHandler(EditorBase::OnCharAdded));
}

void EditorBase::Clear(){
	this->editor->ClearAll();
	this->editor->EmptyUndoBuffer();
	this->title->Clear();
}

void EditorBase::LoadFile(wxString filename){
	wxFFile file;
	if(file.Open(filename, "r")){
		wxString content;
		file.ReadAll(&content);
		this->editor->SetText(content);
		this->editor->DocumentStart();
		this->editor->EmptyUndoBuffer();
		this->title->SetValue(filename);
	}
}

void EditorBase::SaveFile(wxString filename){
	wxFFile file;
	if(file.Open(filename, "w")){
		file.Write(this->editor->GetText(), wxMBConvUTF8());
		this->title->SetValue(filename);
		this->check->Set(true);
	}
}

void EditorBase::SetDefaultStyle(){
	// Style clear
	this->editor->StyleClearAll();
	// Indentation style
	this->editor->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE,   wxColour(196,196,196));
	this->editor->StyleSetBackground(wxSTC_STYLE_INDENTGUIDE,   wxColour(255,255,255));
	// Line numbers style
	this->editor->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour (64, 64, 64) );
	this->editor->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour (225, 225, 225));
}

// Define event handlers
void EditorBase::OnModify(wxStyledTextEvent& event){
	this->check->Set(false);
}

void EditorBase::OnCharAdded(wxStyledTextEvent& event){
	int curLine = this->editor->GetCurrentLine();
	if (curLine > 0 && static_cast<char>(event.GetKey()) == '\n'){
		int indent = this->editor->GetLineIndentation(curLine-1);
		if(indent > 0){
			this->editor->SetLineIndentation(curLine, indent);
			this->editor->GotoPos(this->editor->GetLineIndentPosition(curLine));
		}
	}
}
