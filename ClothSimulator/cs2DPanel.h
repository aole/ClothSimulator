#pragma once

#include "glincludes.h"

#include <Model.h>
#include <ViewListener.h>
#include <View.h>

#include "wx/wx.h"
#include "wx/overlay.h"
#include <wx/graphics.h>

class cs2DPanel : public wxWindow, public ModelListener, public View2D
{
public:
	cs2DPanel(Model* model, wxWindow* parent);

public:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground(wxEraseEvent& event) {}

	void OnMouseMove(wxMouseEvent& event);
	void OnLeftMouseDown(wxMouseEvent& event);
	void OnLeftMouseUp(wxMouseEvent& event);
	void OnMiddleMouseDown(wxMouseEvent& event);
	void OnMiddleMouseUp(wxMouseEvent& event);
	void OnCaptureLost(wxMouseCaptureLostEvent& event);

	void updated() { Refresh(false); };

	void drawTemporaryRectangle(float minx, float miny, float maxx, float maxy) override;
	void setHighlightedPoints(std::vector<Vector2*>& points) override;
	void setSelectedPoints(std::vector<Vector2*>& points) override;

	void pan(float dx, float dy) override;
	void update() override { Refresh(); }

	void setImage(wxImage& image) override;
	void hideImage() override { m_display_image = false; }

private:
	Model* m_model;

	bool m_initialized = false;
	int m_panx, m_pany;

	std::vector<Vector2*> highlight_points;
	std::vector<Vector2*> select_points;

	wxOverlay m_overlay;

	wxGraphicsBitmap m_image;
	bool m_display_image;
	int m_image_width, m_image_height;

	wxDECLARE_EVENT_TABLE();
};
