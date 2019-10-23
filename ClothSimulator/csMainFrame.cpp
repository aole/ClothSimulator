
#include <wx/wx.h>
#include <wx/splitter.h>
#include <Model.h>

#include "csMainFrame.h"
#include "csGLPanel.h"
#include "cs2DPanel.h"
#include <Controller.h>

#include <wx/tglbtn.h>

BEGIN_EVENT_TABLE(csMainFrame, wxFrame)
	EVT_CLOSE(csMainFrame::OnClose)

	EVT_CHAR_HOOK(csMainFrame::OnKeyDown)
END_EVENT_TABLE()

csMainFrame::csMainFrame(Model* model, Controller* controller) : m_model(model), m_controller(controller), wxFrame(nullptr, wxID_ANY, "Cloth Simulator", wxPoint(100,100), wxSize(800, 600))
{
	CreateMenuBar();
	CreateToolStatusBar();
	CreatePanels();
}

void csMainFrame::OnKeyDown(wxKeyEvent& event)
{
	m_controller->keyDown(event.GetKeyCode());
}

void csMainFrame::CreateMenuBar()
{
	wxMenu* menuFile = new wxMenu;

	menuFile->Append(wxID_EXIT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");

	SetMenuBar(menuBar);

	Bind(wxEVT_MENU, [=](wxCommandEvent&) { m_controller->OnMenuFileExit(this); }, wxID_EXIT);
}

void csMainFrame::CreateToolStatusBar()
{
	wxToolBar* toolbar = CreateToolBar();

	wxToggleButton* btnSimulate = new wxToggleButton(toolbar, 20000, "Simulate");
	Bind(wxEVT_TOGGLEBUTTON, [=](wxCommandEvent&) { m_controller->OnToggleSimulation(btnSimulate->GetValue()); }, 20000);
	toolbar->AddControl(btnSimulate);

	wxButton* btnReset = new wxButton(toolbar, 20001, "Reset");
	Bind(wxEVT_BUTTON, [=](wxCommandEvent&) { m_controller->OnRestCloth(); }, 20001);
	toolbar->AddControl(btnReset);

	wxButton* btnPin = new wxButton(toolbar, 20002, "(Un)Pin");
	Bind(wxEVT_BUTTON, [=](wxCommandEvent&) { m_controller->OnPinPoint(); }, 20002);
	toolbar->AddControl(btnPin);

	toolbar->Realize();
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
