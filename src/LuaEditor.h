#pragma once

#include "EditorBase.h"

class LuaEditor : public EditorBase{
	public:
		// Panel initialization
		LuaEditor(wxWindow *wnd);
	private:
		// Event IDs
		enum{MARGIN_FOLD=2};
		// Event handlers
		void OnMarginClick(wxStyledTextEvent& event);
		void OnBrace(wxStyledTextEvent& event);
};