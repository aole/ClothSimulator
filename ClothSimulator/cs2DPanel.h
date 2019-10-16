#pragma once

#include "wx/overlay.h"
#include <Model.h>
#include <ViewListener.h>
#include <View.h>

class cs2DPanel : public wxWindow, public ModelListener, public View
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

private:
	void drawGrid(wxDC &dc);

	Model* m_model;

	bool initialized = false;
	long panx, pany;

	bool m_mouse_left_down;

	long m_lastx, m_lasty;
	wxPoint m_anchorpoint;
	wxOverlay m_overlay;

	wxDECLARE_EVENT_TABLE();
};
