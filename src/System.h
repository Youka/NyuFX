#pragma once

#include <wx/log.h>
#include <wx/intl.h>
#include <wx/tooltip.h>
#include <windows.h>

// En-/Disable logging (and displaying) of errors & warning
#define EnableLogging(status) wxLog::EnableLogging(status)

static void SetLanguage(wxLanguage language){
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

static void ConfigTooltips(long wait_before, long duration, long wait_between, int max_width){
	if(duration > 0){	// Tooltips have a duration?
		wxToolTip::Enable(true);	// Enable tooltips
		wxToolTip::SetDelay(wait_before);	// Duration before tooltip shows
		wxToolTip::SetAutoPop(duration);	// Duration of tooltip visibility
		wxToolTip::SetReshow(wait_between);	// Duration before next tooltip shows
		wxToolTip::SetMaxWidth(max_width);	// Maximal pixel width of tooltips
	}else
		wxToolTip::Enable(false);	// Disable tooltips
}

static bool wxGetShortPathName(wxString in, wxString *out){
	// Get windows compatible string
	const wchar_t *wpath = in.wc_str();
	// Get short path length
	DWORD len = GetShortPathNameW(wpath, NULL, 0);
	if(len > 0){
		// Convert long path to short path
		wxScopedPtr<wchar_t> buffer(new wchar_t[len]);
		if(GetShortPathNameW(wpath, buffer.get(), len) > 0){
			// Save result to target
			*out = buffer.get();
			return true;
		}else
			return false;
	}else
		return false;
}