#include <wx/app.h>
#include "GUI.h"

// Application instance
class NyuFX : public wxApp{
    public:
        bool OnInit(){
			GUI* app_wnd = new GUI;	// Create toplevel window
			app_wnd->Show();	// Activate window
			this->SetTopWindow(app_wnd);	// Set top window for application run
            return true;	// Everything ok, run application
        }
};

IMPLEMENT_APP(NyuFX);	// wxWidgets application initialization