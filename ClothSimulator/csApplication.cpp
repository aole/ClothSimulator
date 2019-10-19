
#include "csApplication.h"
#include "csMainFrame.h"

#include <wx/wx.h>

wxIMPLEMENT_APP(csApplication);

csApplication::~csApplication()
{
	delete m_controller;
	delete m_model;
}

void csApplication::activateRenderLoop(bool on)
{
	if (on && !render_loop_on)
	{
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(csApplication::OnIdle));
		render_loop_on = true;
	}
	else if (!on && render_loop_on)
	{
		Disconnect(wxEVT_IDLE, wxIdleEventHandler(csApplication::OnIdle));
		render_loop_on = false;
	}
}

bool csApplication::OnInit()
{
	render_loop_on = false;

	m_model = new Model();

	m_controller = new Controller(m_model);

	csMainFrame* m_mainFrame;
	m_mainFrame = new csMainFrame(m_model, m_controller);

	m_mainFrame->Show();

	return true;
}

void csApplication::OnIdle(wxIdleEvent& evt)
{
	if (render_loop_on)
	{
		m_controller->simulate();
		evt.RequestMore(); // render continuously, not only once on idle
	}
}
