#include "savesymbol.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

SaveSymbol::SaveSymbol(wxWindow *wnd, wxPoint pos, wxSize size) : wxPanel(wnd, wxID_ANY, pos, size){
	this->SetBackgroundColour(wxColor(240,240,240));
	this->Set(false);
	this->SetToolTip(_("File saved?"));
}

// Bind event IDs to event handlers
BEGIN_EVENT_TABLE(SaveSymbol, wxPanel)
	EVT_PAINT(SaveSymbol::OnPaint)
END_EVENT_TABLE()

// Define event handlers
void SaveSymbol::OnPaint(wxPaintEvent& event){
	wxPaintDC dc(this);
	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	if(gc){
		int sz = this->GetSize().y-3;
		gc->SetBrush(wxBrush(this->color));
		gc->SetPen(wxPen(*wxBLACK, 2, wxSOLID));
		gc->DrawRoundedRectangle(4,2,sz,sz,180);
		delete gc;
	}
}

// Set properties
void SaveSymbol::BackgroundColor(wxColor bgcolor){
	this->SetBackgroundColour(bgcolor);
	this->Refresh();
}

void SaveSymbol::Set(bool check){
	if(check)
		this->color.Set(0,255,0);
	else
		this->color.Set(0,96,0);
	this->Refresh();
}