
#include <wx/wx.h>
#include <wx/splitter.h>

#include "csMainFrame.h"
#include "csGLPanel.h"

csMainFrame::csMainFrame() : wxFrame(nullptr, wxID_ANY, "Cloth Simulator")
{
	CreateMenuBar();
	CreatePanels();
}

void csMainFrame::CreateMenuBar()
{
	wxMenu* menuFile = new wxMenu;

	menuFile->Append(wxID_EXIT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");

	SetMenuBar(menuBar);

	//Bind(wxEVT_MENU, &csMainFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);
}

void csMainFrame::CreatePanels()
{
	wxSplitterWindow* splitter = new wxSplitterWindow(this);

	// ========== CLOTH EDIT PANEL ==========
	wxWindow* window2D = new wxWindow(splitter, wxID_ANY);
	window2D->SetBackgroundColour(wxColour(150, 150, 150));

	// ========== 3D OPENGL PANEL ==========
	csGLPanel* window3D = new csGLPanel(splitter);

	// ========== SPLITTER ==========
	splitter->SetSize(GetClientSize());
	splitter->SetSashGravity(0.5);
	splitter->SplitVertically(window3D, window2D);
}

void csMainFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}
