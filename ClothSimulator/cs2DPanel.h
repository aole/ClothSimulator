#pragma once

#include "glincludes.h"

#include <Model.h>
#include <ViewListener.h>
#include <View.h>

#include "wx/wx.h"
#include "wx/overlay.h"

class cs2DPanel : public wxWindow, public ModelListener, public View2D
{
public:
	cs2DPanel(Model* model, wxWindow* parent);

public:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);

	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);

	void updated() { Refresh(false); };

	void drawTemporaryRectangle(float minx, float miny, float maxx, float maxy);

private:
	void drawGrid(wxDC &dc);

	Model* m_model;

	bool m_initialized = false;
	long m_panx, m_pany;

	wxOverlay m_overlay;

	wxDECLARE_EVENT_TABLE();
};
