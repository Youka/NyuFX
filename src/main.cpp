#include <wx/app.h>
#include "GUI.h"

// Application instance
class NyuFX : public wxApp{
    public:
        bool OnInit(){
			GUI* app_wnd = new GUI(this->argv.GetArguments());	// Create single application window instance for whole running
			app_wnd->Show();	// Activate window
			this->SetTopWindow(app_wnd);	// Set os focus on window
            return true;	// Everything ok, run application
        }
};

IMPLEMENT_APP(NyuFX);	// wxWidgets application initialization