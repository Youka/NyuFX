#pragma once

#include <wx/panel.h>

class SaveSymbol : public wxPanel{
	public:
		// Panel initialization
		SaveSymbol(wxWindow *wnd, wxPoint pos, wxSize size);
		// Properties setting
		void Set(bool check);
		void BackgroundColor(wxColor bgcolor);
	private:
		// Lamp color
		wxColor color;
		// Event handlers
		DECLARE_EVENT_TABLE()
		void OnPaint(wxPaintEvent& event);
};
