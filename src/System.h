#pragma once

#include <wx/intl.h>
#include <wx/tooltip.h>

void SetLanguage(wxLanguage language){
	wxLocale *lang = new wxLocale(language, wxLOCALE_CONV_ENCODING);	// Allocate language instance & set current language
	lang->AddCatalogLookupPathPrefix(wxT("lang"));	// Set lookup directory for languages
	lang->AddCatalog(wxT("nyufx"));	// Define catalog name / language file name to load
	if(!lang->IsOk())	// If language isn't available...
		new wxLocale(wxLANGUAGE_ENGLISH);	// ... go back to secure english language
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