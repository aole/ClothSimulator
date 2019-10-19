
#include "cs2DPanel.h"

#include "wx/dcbuffer.h"
#include <wx/graphics.h>

wxBEGIN_EVENT_TABLE(cs2DPanel, wxWindow)
	EVT_SIZE(cs2DPanel::OnSize)
	EVT_PAINT(cs2DPanel::OnPaint)
	EVT_MOTION(cs2DPanel::OnMouseMove)
	EVT_LEFT_DOWN(cs2DPanel::OnMouseDown)
	EVT_LEFT_UP(cs2DPanel::OnMouseUp)
wxEND_EVENT_TABLE()

cs2DPanel::cs2DPanel(Model* model, wxWindow* parent) : m_model(model), wxWindow(parent, wxID_ANY), m_panx(0), m_pany(0)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
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
	wxAutoBufferedPaintDC dc(this);
	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	dc.Clear();

	// DRAW GRID
	wxRect r = GetClientRect();
	gc->SetBrush(*wxTRANSPARENT_BRUSH);
	gc->SetPen(wxPen(wxColour(155, 50, 50, 50), 1));
	// x axis
	gc->StrokeLine(0, m_pany, r.width, m_pany);
	gc->SetPen(wxPen(wxColour(50, 155, 50, 50), 1));
	gc->StrokeLine(m_panx, 0, m_panx, r.height);

	gc->SetPen(*wxMEDIUM_GREY_PEN);
	gc->SetBrush(*wxLIGHT_GREY_BRUSH);

	// DRAW CLOTHES
	wxGraphicsPath path = gc->CreatePath();
	for (ClothShape* s : m_model->getShapes()) {
		if (s->getCount() > 2) {
			path.MoveToPoint(s->getVertex(0).x + m_panx, -s->getVertex(0).y + m_pany);
			for (int i = 1; i < s->getCount(); i++) {
				path.AddLineToPoint(s->getVertex(i).x + m_panx, -s->getVertex(i).y + m_pany);
			}
			path.CloseSubpath();
		}
	}
	gc->FillPath(path);
	gc->StrokePath(path);

	delete gc;
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
