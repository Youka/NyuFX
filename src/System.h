#pragma once

#include <wx/log.h>
#include <wx/language.h>

// En-/Disable logging (and displaying) of errors & warning
#define EnableLogging(status) wxLog::EnableLogging(status)

void SetLanguage(wxLanguage language);
void ConfigTooltips(long wait_before, long duration, long wait_between, int max_width);