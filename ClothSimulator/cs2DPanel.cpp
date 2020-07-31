
#include "cs2DPanel.h"

#include "wx/dcbuffer.h"
#include <wx/graphics.h>

bool DEBUG_DISPLAY_VERTEX_NUMBER = false;

wxBEGIN_EVENT_TABLE(cs2DPanel, wxWindow)
EVT_SIZE(cs2DPanel::OnSize)
EVT_PAINT(cs2DPanel::OnPaint)
EVT_MOTION(cs2DPanel::OnMouseMove)
EVT_LEFT_DOWN(cs2DPanel::OnLeftMouseDown)
EVT_LEFT_UP(cs2DPanel::OnLeftMouseUp)
EVT_MIDDLE_DOWN(cs2DPanel::OnMiddleMouseDown)
EVT_MIDDLE_UP(cs2DPanel::OnMiddleMouseUp)
EVT_MOUSE_CAPTURE_LOST(cs2DPanel::OnCaptureLost)
EVT_ERASE_BACKGROUND(cs2DPanel::OnEraseBackground)
wxEND_EVENT_TABLE()

bool mouse_captured = false;

cs2DPanel::cs2DPanel(Model* model, wxWindow* parent) : m_model(model), wxWindow(parent, wxID_ANY), m_panx(0), m_pany(0), m_display_image(false), m_image_width(0), m_image_height(0)
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

	gc->SetBrush(*wxGREY_BRUSH);

	wxPen penPin = *wxThePenList->FindOrCreatePen(wxColour(200, 50, 50), 2);

	// DRAW CLOTHES
	for (auto s : m_model->getShapes()) {
		gc->SetPen(*wxMEDIUM_GREY_PEN);
		// draw polygons
		for (auto poly : s->m_polygons) {
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
				gc->SetPen(penPin);
			else
				gc->SetPen(*wxMEDIUM_GREY_PEN);

			if (DEBUG_DISPLAY_VERTEX_NUMBER) {
				wxString vs;
				vs << i;
				dc.DrawText(vs, v->x + (double)m_panx - 2, -v->y + (double)m_pany - 2);
			}
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

	if (m_display_image) {
		// place the image in the middle x and over y
		gc->DrawBitmap(m_image, (int)m_panx - m_image_width / 2, (int)m_pany - m_image_height, m_image_width, m_image_height);
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

void cs2DPanel::pan(float dx, float dy)
{
	//wxLogDebug("pan by: %f, %f", dx, dy);
	m_panx += dx; m_pany += dy; update();
}

void cs2DPanel::setImage(wxImage& image)
{
	if (!image.HasAlpha())
		image.InitAlpha();

	byte* alphabuf = new byte[image.GetWidth() * image.GetHeight()];
	std::fill_n(alphabuf, image.GetWidth() * image.GetHeight(), 100);
	image.SetAlpha(alphabuf);

	wxClientDC dc(this);
	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
	m_image = gc->CreateBitmapFromImage(image);
	m_image_width = image.GetWidth();
	m_image_height = image.GetHeight();

	m_display_image = true;

	delete gc;
}

void cs2DPanel::OnMouseMove(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	wxClientDC dc(this);

	int x = dc.DeviceToLogicalX(pos.x);
	int y = dc.DeviceToLogicalY(pos.y);

	//wxLogDebug("Mouse move: %i, %i", x - m_panx, -(y - m_pany));
	for (ViewListener* l : m_listeners)
		l->mouseMove2D(pos.x, pos.y, x - m_panx, -(y - m_pany));
}

void cs2DPanel::OnLeftMouseDown(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	wxClientDC dc(this);

	if (!this->HasCapture())
		CaptureMouse();

	long x = dc.DeviceToLogicalX(pos.x);
	long y = dc.DeviceToLogicalY(pos.y);

	for (ViewListener* l : m_listeners)
		l->leftMouseDown2D(pos.x, pos.y, x - m_panx, -(y - m_pany));
}

void cs2DPanel::OnLeftMouseUp(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();

	if (this->HasCapture())
		ReleaseMouse();

	{
		wxClientDC dc(this);

		long x = dc.DeviceToLogicalX(pos.x);
		long y = dc.DeviceToLogicalY(pos.y);

		for (ViewListener* l : m_listeners)
			l->leftMouseUp2D(pos.x, pos.y, x - m_panx, -(y - m_pany));

	} // use xcurly braces or reset will fail assert

	m_overlay.Reset();
}

void cs2DPanel::OnMiddleMouseDown(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	wxClientDC dc(this);

	if (!this->HasCapture())
		CaptureMouse();

	long x = dc.DeviceToLogicalX(pos.x);
	long y = dc.DeviceToLogicalY(pos.y);

	for (ViewListener* l : m_listeners)
		l->middleMouseDown2D(pos.x, pos.y, x - m_panx, -(y - m_pany));
}

void cs2DPanel::OnMiddleMouseUp(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();

	if (this->HasCapture())
		ReleaseMouse();

	{
		wxClientDC dc(this);

		long x = dc.DeviceToLogicalX(pos.x);
		long y = dc.DeviceToLogicalY(pos.y);

		for (ViewListener* l : m_listeners)
			l->middleMouseUp2D(pos.x, pos.y, x - m_panx, -(y - m_pany));

	} // use xcurly braces or reset will fail assert

	m_overlay.Reset();
}

void cs2DPanel::OnCaptureLost(wxMouseCaptureLostEvent& event)
{
	mouse_captured = false;
}
