#pragma once

#include "wx/wx.h"

class cs2DPanel : public wxWindow
{
public:
	cs2DPanel(wxWindow* parent);

public:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);

	wxDECLARE_EVENT_TABLE();
};