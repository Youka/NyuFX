#pragma once

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/gauge.h>
#include <wx/button.h>

class OutputCtrl : public wxPanel{
	public:
		// Window initialization
		OutputCtrl(wxWindow *wnd);
		// Subwindow sizers
		wxBoxSizer *v_box;
		// Subwindows
		wxStaticBitmap *logo;
		wxStaticText *log_text, *progress_text, *input_text, *after_proc;
		wxTextCtrl *log, *out_file, *cmd;
		wxGauge *progressbar;
		wxPanel *input_panel;
		wxButton *choose, *gencanc, *open;
	private:
		// Interface construction
		void CreateElements();
		void PlaceElements();
		// Event IDs
		enum{
			ID_OUT_CHOOSE_FILE = 100,
			ID_OUT_GENCANC,
			ID_OUT_OPEN
		};
		// Event handlers
		DECLARE_EVENT_TABLE()
		void OnChooseFile(wxCommandEvent& event);
		void OnGenerateCancel(wxCommandEvent& event);
		void OnOpen(wxCommandEvent& event);
		void OnAfterProcess(wxThreadEvent& event);
};