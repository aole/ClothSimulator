#pragma once

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