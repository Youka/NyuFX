#pragma once

#include <wx/log.h>
#include <wx/intl.h>
#include <wx/tooltip.h>

// Disable logging (and displaying) of errors & warning
#define EnableLogging(status) wxLog::EnableLogging(status)

void SetLanguage(wxLanguage language){
	// Locale memory
	static wxLocale *locale = 0;
	if(locale)
		delete locale;
	locale = new wxLocale(language, wxLOCALE_CONV_ENCODING);	// Set current language
	locale->AddCatalogLookupPathPrefix(wxT("lang"));	// Set lookup directory for languages
	locale->AddCatalog(wxT("nyufx"));	// Define catalog name / language file name to load
	if(!locale->IsOk()){	// If language isn't available...
		delete locale;
		locale = new wxLocale(wxLANGUAGE_ENGLISH);	// ... go back to secure english language
	}
}

void ConfigTooltips(long wait_before, long duration, long wait_between, int max_width){
	if(duration > 0){	// Tooltips have a duration?
		wxToolTip::Enable(true);	// Enable tooltips
		wxToolTip::SetDelay(wait_before);	// Duration before tooltip shows
		wxToolTip::SetAutoPop(duration);	// Duration of tooltip visibility
		wxToolTip::SetReshow(wait_between);	// Duration before next tooltip shows
		wxToolTip::SetMaxWidth(max_width);	// Maximal pixel width of tooltips
	}else
		wxToolTip::Enable(false);	// Disable tooltips
}