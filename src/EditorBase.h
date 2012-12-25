#pragma once

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stc/stc.h>
#include "savesymbol.h"

class EditorBase : public wxPanel{
	public:
		// Panel initialization
		EditorBase(wxWindow* wnd);
		// Load file into editor
		void LoadFile(wxString filename);
		// Save editor to file
		void SaveFile(wxString filename);
		// Layout
		wxBoxSizer *v_box, *h_box;
		// Subwindows
		SaveSymbol *check;
		wxTextCtrl *title;
		wxStyledTextCtrl *editor;
	private:
		// Interface construction
		void CreateElements();
		void PlaceElements();
		enum{MARGIN_LINE_NUMBERS=1};
		void ConfigureEditor();
		// Event handlers
		void OnModify(wxStyledTextEvent& event);
		void OnCharAdded(wxStyledTextEvent& event);
	protected:
		// Macro for default style setting
		void SetDefaultStyle();
};