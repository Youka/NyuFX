#pragma once

#include <wx/taskbar.h>
#include <wx/frame.h>
#include <wx/menu.h>

class TaskIcon : public wxTaskBarIcon{
    public:
		// Icon initialization
        TaskIcon(wxFrame *parent);
    private:
		// Parent window handle
        wxFrame *parent;
		// Event handlers
        DECLARE_EVENT_TABLE()
        void OnTaskLDBClick(wxTaskBarIconEvent& event);
        void OnClose(wxCommandEvent& event);
    protected:
		// Override of menu creation
        wxMenu* CreatePopupMenu();
};
