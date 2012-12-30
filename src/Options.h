#pragma once

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/fontpicker.h>
#include <wx/button.h>

class Options : public wxDialog{
	public:
		// Window initialization
		Options(wxWindow *wnd);
		// Layout
		wxBoxSizer *v_box;
		wxFlexGridSizer *flex;
		// Subwindows
		wxStaticText *language_label, *sound_label, *m2i_label, *font_label;
		wxComboBox *languages;
		wxTextCtrl *sound_file;
		wxCheckBox *minimize2icon;
		wxFontPickerCtrl *font;
		wxButton *close;
	private:
		// Event handlers
		DECLARE_EVENT_TABLE()
		void OnClose(wxCloseEvent& event);
};
