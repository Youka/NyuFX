#include <wx/app.h>
#include <wx/intl.h>
#include "GUI.h"

class NyuFX : public wxApp{
    public:
        bool OnInit(){
            // Language
            static wxLocale lang(wxLANGUAGE_ENGLISH);

            // Application window
			GUI* main_wnd = new GUI(this->argv.GetArguments());
            main_wnd->Show();
            SetTopWindow(main_wnd);

            return true;
        }
};

IMPLEMENT_APP(NyuFX);