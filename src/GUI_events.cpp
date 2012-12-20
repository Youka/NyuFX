#include "GUI.h"
#include "Config.h"

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(GUI, wxFrame)
	EVT_CLOSE(GUI::OnClose)
	// OnTool defined during menu creation
END_EVENT_TABLE()

// Define event handlers
void GUI::OnClose(wxCloseEvent &event){
    // Check for running generation
	Config::Save();
    this->Destroy();
}

void GUI::OnTool(wxCommandEvent& event){
    wxString tool = this->tools[event.GetId()-ID_MENU_TOOL];
    if( tool.AfterLast('.') == "exe" )
        wxExecute( tool );
    else
        wxLaunchDefaultApplication( tool );
}