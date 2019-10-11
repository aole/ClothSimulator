#pragma once

#include "wx/wx.h"

class csMainFrame : public wxFrame
{
public:
	csMainFrame();

public:
	void CreateMenuBar();
	void CreatePanels();
public:
	void OnExit(wxCommandEvent& event);
};