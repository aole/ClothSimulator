#include "cs2DPanel.h"
#include "wx/dcbuffer.h"

wxBEGIN_EVENT_TABLE(cs2DPanel, wxWindow)
	EVT_SIZE(cs2DPanel::OnSize)
	EVT_PAINT(cs2DPanel::OnPaint)
wxEND_EVENT_TABLE()

cs2DPanel::cs2DPanel(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetBackgroundColour(wxColour(204, 204, 204));
	SetWindowStyle(wxBORDER_SUNKEN);
}

void cs2DPanel::OnSize(wxSizeEvent& event)
{
	event.Skip();
	Refresh(false);
}

void cs2DPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);
	
	dc.Clear();
	dc.SetPen(*wxMEDIUM_GREY_PEN);
	for (int i = 0; i < 200; i++)
		dc.DrawLine(0, i * 10, i * 10, 0);

}
