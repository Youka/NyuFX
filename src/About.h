#pragma once

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/statbmp.h>

class About : public wxDialog{
	public:
		// Window initialization
		About(wxWindow *wnd);
		// Subwindow handles
		wxPanel *panel;
		wxBoxSizer *v_box;
		wxStaticBitmap *pic;
		wxTextCtrl *text;
		wxButton *ok;
	private:
		// Event IDs
		enum{ID_URL_CLICK = 100};
		// Event handlers
		DECLARE_EVENT_TABLE();
		void OnURLClick(wxTextUrlEvent &url);
};
