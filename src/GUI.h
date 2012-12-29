#pragma once

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include "TaskIcon.h"
#include "LuaEditor.h"
#include "ASSEditor.h"
#include "OutputCtrl.h"

// Main window
class GUI : public wxFrame{
	public:
		// Window initialization
		GUI();
		// Clean resources
		~GUI();
		// Menu handles
		wxMenu *fileMenu, *editMenu, *viewMenu, *editviewMenu, *openviewMenu, *resetMenu, *toolMenu, *helpMenu;
		// System tray
		TaskIcon *taskicon;
		// Layout
		wxBoxSizer *h_box;
		wxSplitterWindow *splitter;
		// Subwindows
		LuaEditor *lua_editor;
		ASSEditor *ass_editor;
		OutputCtrl *output_panel;
		// Application files in tools folder
		wxArrayString tools;
	private:
		// Interface construction
		void SetMeta();
		void CreateMenu();
		void CreateElements();
		void PlaceElements();
		void ReadConfig();
        // Event IDs
		enum{
			ID_MENU_ACTIVE = 100,
			ID_MENU_NEW,
			ID_MENU_OPEN,
			ID_MENU_RELOAD,
			ID_MENU_SAVE,
			ID_MENU_SAVE_AS,
			ID_MENU_QUIT,

			ID_MENU_UNDO = 200,
			ID_MENU_REDO,
			ID_MENU_CUT,
			ID_MENU_COPY,
			ID_MENU_PASTE,
			ID_MENU_DELETE,
			ID_MENU_SELECT_ALL,
			ID_MENU_REPLACE,
			ID_MENU_AUTO_STYLE,

			ID_MENU_VIEW_LUA = 300,
			ID_MENU_VIEW_ASS,
			ID_MENU_GOTO,
			ID_MENU_ZOOM_IN,
			ID_MENU_ZOOM_OUT,
			ID_MENU_FOLD_ALL,
			ID_MENU_UNFOLD_ALL,
			ID_MENU_OPEN_INCL,
			ID_MENU_OPEN_TMPL,
			ID_MENU_OPEN_TOOLS,
			ID_MENU_OPEN_OPTIONS,

			ID_MENU_CLEAR_LOG = 400,
			ID_MENU_RESET_PROG,

			ID_MENU_TOOL = 500,

			ID_MENU_HELP = 600,
			ID_MENU_ABOUT
		};
		// Event handlers
		DECLARE_EVENT_TABLE()
		void OnMenuOpen(wxMenuEvent& event);
		void OnIconize(wxIconizeEvent& event);
		void OnClose(wxCloseEvent& event);

		void OnNew(wxCommandEvent& event);
		void OnOpen(wxCommandEvent& event);
		void OnReload(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnSaveAs(wxCommandEvent& event);
		void OnQuit(wxCommandEvent& event);

		void OnUndo(wxCommandEvent& event);
		void OnRedo(wxCommandEvent& event);
		void OnCut(wxCommandEvent& event);
		void OnCopy(wxCommandEvent& event);
		void OnPaste(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
		void OnSelectAll(wxCommandEvent& event);
		void OnReplace(wxCommandEvent& event);
		void OnAutoStyle(wxCommandEvent& event);

		void OnViewLua(wxCommandEvent& event);
		void OnViewASS(wxCommandEvent& event);
		void OnGotoLine(wxCommandEvent& event);
		void OnZoomIn(wxCommandEvent& event);
		void OnZoomOut(wxCommandEvent& event);
		void OnFoldAll(wxCommandEvent& event);
		void OnUnfoldAll(wxCommandEvent& event);
		void OnOpenInclude(wxCommandEvent& event);
		void OnOpenTemplates(wxCommandEvent& event);
		void OnOpenTools(wxCommandEvent& event);
		void OnOpenOptions(wxCommandEvent& event);

		void OnClearLog(wxCommandEvent& event);
		void OnResetProgressbar(wxCommandEvent& event);

		void OnTool(wxCommandEvent& event);

		void OnHelp(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
};
