#pragma once

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/textctrl.h>
#include <wx/button.h>

class About : public wxDialog{
	public:
		// Window initialization
		About(wxWindow *wnd);
		// Layout
		wxBoxSizer *v_box;
		// Subwindow handles
		wxStaticBitmap *pic;
		wxTextCtrl *text;
		wxButton *ok;
	private:
		// Interface construction
		void CreateElements();
		void PlaceElements();
		// Event handlers
		DECLARE_EVENT_TABLE();
		void OnURLClick(wxTextUrlEvent &url);
};
