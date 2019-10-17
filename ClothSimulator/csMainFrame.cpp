
#include <wx/wx.h>
#include <wx/splitter.h>
#include <Model.h>

#include "csMainFrame.h"
#include "csGLPanel.h"
#include "cs2DPanel.h"
#include <Controller.h>

csMainFrame::csMainFrame(Model* model, Controller* controller) : m_model(model), m_controller(controller), wxFrame(nullptr, wxID_ANY, "Cloth Simulator", wxPoint(100,100), wxSize(800, 600))
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
	m_2DView = new cs2DPanel(m_model, splitter);
	m_controller->add2DView(m_2DView);

	// ========== 3D OPENGL PANEL ==========
	wxGLAttributes vAttrs;
	vAttrs.PlatformDefaults().Defaults().EndList();

	m_3DView = new csGLPanel(m_model, splitter, vAttrs);

	// ========== SPLITTER ==========
	splitter->SetSize(GetClientSize());
	splitter->SetSashGravity(0.5);
	splitter->SplitVertically(m_3DView, m_2DView);
}

void csMainFrame::addViewListener(ViewListener* l)
{
	m_2DView->addViewListener(l);
}

void csMainFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}
