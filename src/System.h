#pragma once

#include <wx/log.h>
#include <wx/intl.h>
#include <wx/tooltip.h>

// En-/Disable logging (and displaying) of errors & warning
#define EnableLogging(status) wxLog::EnableLogging(status)

static void SetLanguage(wxLanguage language){
	// Set locale to english (wx sets default locale to system at startup and updates it over time, so wxLocale is needed for an effective change)
	static wxLocale lang(wxLANGUAGE_ENGLISH);
	// Create & define translation object
	wxFileTranslationsLoader::AddCatalogLookupPathPrefix(wxT("lang"));
	wxTranslations *trans = new wxTranslations;
	trans->SetLanguage(language);
	trans->AddStdCatalog();
	trans->AddCatalog(wxT("nyufx"));
	// Set current translator and delete previous
	wxTranslations::Set(trans);
}

inline void ConfigTooltips(long wait_before, long duration, long wait_between, int max_width){
	if(duration > 0){	// Tooltips have a duration?
		wxToolTip::Enable(true);	// Enable tooltips
		wxToolTip::SetDelay(wait_before);	// Duration before tooltip shows
		wxToolTip::SetAutoPop(duration);	// Duration of tooltip visibility
		wxToolTip::SetReshow(wait_between);	// Duration before next tooltip shows
		wxToolTip::SetMaxWidth(max_width);	// Maximal pixel width of tooltips
	}else
		wxToolTip::Enable(false);	// Disable tooltips
}