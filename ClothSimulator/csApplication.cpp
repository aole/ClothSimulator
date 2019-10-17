
#include "csApplication.h"

#include <wx/wx.h>

wxIMPLEMENT_APP(csApplication);

csApplication::~csApplication()
{
	delete m_controller;
	delete m_model;
}

bool csApplication::OnInit()
{
	m_model = new Model();

	m_controller = new Controller(m_model);

	m_mainFrame = new csMainFrame(m_model, m_controller);

	m_mainFrame->Show();

	return true;
}
