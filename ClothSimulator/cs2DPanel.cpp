
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
		m_pany = r.height - r.height / 4;

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
	gc->SetPen(wxPen(wxColour(155, 50, 50, 100), 1));
	// x axis
	gc->StrokeLine(0, m_pany, r.width, m_pany);
	gc->SetPen(wxPen(wxColour(50, 155, 50, 100), 1));
	gc->StrokeLine(m_panx, 0, m_panx, r.height);

	gc->SetBrush(*wxLIGHT_GREY_BRUSH);

	// DRAW CLOTHES
	for (auto s : m_model->getShapes()) {
		gc->SetPen(*wxMEDIUM_GREY_PEN);
		// draw polygons
		for (auto poly : s->m_polygons) {
			//wxLogDebug("drawing");
			//printpoly(&poly);
			wxGraphicsPath path = gc->CreatePath();
			int idx = poly.indices[0];
			auto v = s->getPoint(idx);
			path.MoveToPoint(v->x + (double)m_panx, -v->y + (double)m_pany);
			for (idx = 1; idx < poly.indices.size(); idx++) {
				v = s->getPoint(poly.indices[idx]);
				path.AddLineToPoint(v->x + (double)m_panx, -v->y + (double)m_pany);
			}
			path.CloseSubpath();
			gc->FillPath(path);
			gc->StrokePath(path);
		}
		for (int i = 0;i < s->getCount();i++) {
			auto v = s->getPoint(i);
			if (v->pin)
				gc->SetPen(wxPen(wxColour(200, 50, 50), 1));
			else
				gc->SetPen(*wxMEDIUM_GREY_PEN);
			wxString vs;
			vs << i;
			dc.DrawText(vs, v->x + (double)m_panx - 2, -v->y + (double)m_pany - 2);
			gc->DrawEllipse(v->x + (double)m_panx - 2, -v->y + (double)m_pany - 2, 4, 4);
		}
	}


	// draw selected vertices
	if (!select_points.empty()) {
		for (auto v : select_points) {
			double x = (double)v->x + m_panx;
			double y = (double)-v->y + m_pany;
			if(v->pin)
				gc->SetPen(wxPen(wxColour(200, 50, 50), 1));
			else
				gc->SetPen(*wxMEDIUM_GREY_PEN);
			gc->DrawEllipse(x - 5, y - 5, 10, 10);
		}
	}
	// draw highlighted vertices
	if (!highlight_points.empty()) {
		for (auto v : highlight_points) {
			double x = (double)v->x + m_panx;
			double y = (double)-v->y + m_pany;
			if (v->pin)
				gc->SetPen(wxPen(wxColour(200, 50, 50), 1));
			else
				gc->SetPen(*wxMEDIUM_GREY_PEN);
			gc->DrawEllipse(x - 5, y - 5, 10, 10);
		}
	}
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

void cs2DPanel::setHighlightedPoints(std::vector<Vector2*>& points)
{
	highlight_points.clear();
	highlight_points = points;

	Refresh();
}

void cs2DPanel::setSelectedPoints(std::vector<Vector2*>& points)
{
	select_points.clear();
	select_points = points;

	Refresh();
}

void cs2DPanel::OnMouseMove(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	wxClientDC dc(this);

	int x = dc.DeviceToLogicalX(pos.x);
	int y = dc.DeviceToLogicalY(pos.y);

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
