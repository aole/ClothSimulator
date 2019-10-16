
#include "wx/wx.h"

#include "wx/dcbuffer.h"

#include "cs2DPanel.h"

wxBEGIN_EVENT_TABLE(cs2DPanel, wxWindow)
	EVT_SIZE(cs2DPanel::OnSize)
	EVT_PAINT(cs2DPanel::OnPaint)
	EVT_MOTION(cs2DPanel::OnMouseMove)
	EVT_LEFT_DOWN(cs2DPanel::OnMouseDown)
	EVT_LEFT_UP(cs2DPanel::OnMouseUp)
wxEND_EVENT_TABLE()

cs2DPanel::cs2DPanel(Model* model, wxWindow* parent) : m_model(model), wxWindow(parent, wxID_ANY), m_mouse_left_down(false), m_lastx(0), m_lasty(0), m_anchorpoint(0, 0)
{
	SetBackgroundColour(wxColour(204, 204, 204));
	SetWindowStyle(wxBORDER_SUNKEN);

	model->addActionListener(this);
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

	for (Shape* s : m_model->getShapes()) {
		if (s->getCount() > 2) {
			glm::vec2 v1 = s->getVertex(s->getCount() - 1);
			for (glm::vec2 v2 : s->getVertices()) {
				dc.DrawLine(v1.x, v1.y, v2.x, v2.y);
				v1 = v2;
			}
		}
	}
}

void cs2DPanel::OnMouseMove(wxMouseEvent& event)
{
	if (m_mouse_left_down) {
		wxPoint pos = event.GetPosition();
		wxClientDC dc(this);
		PrepareDC(dc);

		long x = dc.DeviceToLogicalX(pos.x);
		long y = dc.DeviceToLogicalY(pos.y);

		wxPoint currentpoint = wxPoint(x, y);
		wxRect newrect(m_anchorpoint, currentpoint);

		wxDCOverlay overlaydc(m_overlay, &dc);
		overlaydc.Clear();

		dc.SetPen(wxPen(wxColour(104, 104, 104), 2));
		dc.SetBrush(*wxTRANSPARENT_BRUSH);

		dc.DrawRectangle(newrect);

		m_lastx = x;
		m_lasty = y;
	}
}

void cs2DPanel::OnMouseDown(wxMouseEvent& event)
{
	m_mouse_left_down = true;
	wxPoint pos = event.GetPosition();
	wxClientDC dc(this);

	CaptureMouse();

	m_lastx = dc.DeviceToLogicalX(pos.x);
	m_lasty = dc.DeviceToLogicalY(pos.y);

	m_anchorpoint.x = m_lastx;
	m_anchorpoint.y = m_lasty;
}

void cs2DPanel::OnMouseUp(wxMouseEvent& event)
{
	m_mouse_left_down = false;
	ReleaseMouse();
	{
		wxClientDC dc(this);
		PrepareDC(dc);
		wxDCOverlay overlaydc(m_overlay, &dc);
		overlaydc.Clear();
	} // use xcurly braces or reset will fail assert
	m_overlay.Reset();

	for(ViewListener *l: m_listeners)
		l->rectangleAdded(m_anchorpoint.x, m_anchorpoint.y, m_lastx, m_lasty);
}
