#pragma once

#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/webview.h>
#include <wx/string.h>

class HelpWindow : public wxFrame{
	public:
		// Window initialization
		HelpWindow(wxWindow *wnd);
		// Layout
		wxBoxSizer *main_hbox, *title_hbox, *web_vbox;
		// Subwindows
		wxListBox *entries;
		wxTextCtrl *title;
		wxButton *enter, *stop, *back, *forward;
		wxWebView *web;
		// Help window files reference
		wxArrayString files;
	private:
		// Interface construction
		void CreateElements();
		void PlaceElements();
		void SetContent();
		// Event handlers
		DECLARE_EVENT_TABLE()
		void OnClose(wxCloseEvent& event);
		void CheckHistory();
		void OnSelect(wxCommandEvent& event);
		void OnEnter(wxCommandEvent& event);
		void OnHistoryAction(wxCommandEvent& event);
		void OnLoaded(wxWebViewEvent& event);
};