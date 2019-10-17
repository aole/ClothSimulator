
#include "cs2DPanel.h"

#include "wx/dcbuffer.h"

wxBEGIN_EVENT_TABLE(cs2DPanel, wxWindow)
	EVT_SIZE(cs2DPanel::OnSize)
	EVT_PAINT(cs2DPanel::OnPaint)
	EVT_MOTION(cs2DPanel::OnMouseMove)
	EVT_LEFT_DOWN(cs2DPanel::OnMouseDown)
	EVT_LEFT_UP(cs2DPanel::OnMouseUp)
wxEND_EVENT_TABLE()

cs2DPanel::cs2DPanel(Model* model, wxWindow* parent) : m_model(model), wxWindow(parent, wxID_ANY), m_panx(0), m_pany(0)
{
	SetBackgroundColour(wxColour(204, 204, 204));
	SetWindowStyle(wxBORDER_SUNKEN);

	model->addActionListener(this);
}

void cs2DPanel::OnSize(wxSizeEvent& event)
{
	event.Skip();

	if (!m_initialized) {
		wxRect r = GetClientRect();
		m_panx = r.width / 2;
		m_pany = r.height / 2;

		m_initialized = true;
	}

	Refresh(false);
}

void cs2DPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);

	dc.Clear();

	drawGrid(dc);

	dc.SetPen(*wxMEDIUM_GREY_PEN);
	for (Shape* s : m_model->getShapes()) {
		if (s->getCount() > 2) {
			glm::vec2 v1 = s->getVertex(s->getCount() - 1);
			for (glm::vec2 v2 : s->getVertices()) {
				dc.DrawLine(v1.x + m_panx, -v1.y + m_pany, v2.x + m_panx, -v2.y + m_pany);
				v1 = v2;
			}
		}
	}
}

void cs2DPanel::drawGrid(wxDC& dc)
{
	wxRect r = GetClientRect();
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(wxPen(wxColour(255, 150, 150, 50), 1));
	// x axis
	dc.DrawLine(0, m_pany, r.width, m_pany);
	dc.SetPen(wxPen(wxColour(150, 255, 150, 50), 1));
	dc.DrawLine(m_panx, 0, m_panx, r.height);
}

void cs2DPanel::drawTemporaryRectangle(float minx, float miny, float maxx, float maxy)
{
	wxClientDC dc(this);
	PrepareDC(dc);

	wxDCOverlay overlaydc(m_overlay, &dc);
	overlaydc.Clear();

	dc.SetPen(wxPen(wxColour(104, 104, 104), 2));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	wxRect rect(wxPoint(minx + m_panx, -miny + m_pany), wxPoint(maxx + m_panx, -maxy + m_pany));
	dc.DrawRectangle(rect);
}

void cs2DPanel::OnMouseMove(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	wxClientDC dc(this);

	long x = dc.DeviceToLogicalX(pos.x);
	long y = dc.DeviceToLogicalY(pos.y);

	for (ViewListener* l : m_listeners)
		l->mouseMove2D(pos.x, pos.y, x - m_panx, -(y - m_pany));
}

void cs2DPanel::OnMouseDown(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	wxClientDC dc(this);

	CaptureMouse();

	long x = dc.DeviceToLogicalX(pos.x);
	long y = dc.DeviceToLogicalY(pos.y);

	for (ViewListener* l : m_listeners)
		l->mouseDown2D(pos.x, pos.y, x - m_panx, -(y - m_pany));
}

void cs2DPanel::OnMouseUp(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();

	ReleaseMouse();
	{
		wxClientDC dc(this);

		long x = dc.DeviceToLogicalX(pos.x);
		long y = dc.DeviceToLogicalY(pos.y);

		for (ViewListener* l : m_listeners)
			l->mouseUp2D(pos.x, pos.y, x - m_panx, -(y - m_pany));

	} // use xcurly braces or reset will fail assert

	m_overlay.Reset();
}
