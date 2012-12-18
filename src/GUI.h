#pragma once

#include <wx/frame.h>

class GUI : public wxFrame{
	public:
		GUI(const wxArrayString&);
		~GUI();
};
