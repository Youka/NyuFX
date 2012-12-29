#pragma once

#include <wx/fdrepdlg.h>
#include <wx/stc/stc.h>
#include <wx/menu.h>

class Replace : public wxFindReplaceDialog{
	public:
		// Window initialization
		Replace(wxWindow *wnd, wxFindReplaceData *data, wxStyledTextCtrl *textctrl, wxMenuItem *item);
	private:
		// Object references
		wxStyledTextCtrl *editor;
		wxMenuItem *item;
		// Event handlers
		wxDECLARE_EVENT_TABLE();
		void OnFindAction(wxFindDialogEvent& event);
		void OnFindClose(wxFindDialogEvent& event);
};
