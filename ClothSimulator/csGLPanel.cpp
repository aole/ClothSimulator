
#include "csGLPanel.h"

#include <wx/wx.h>

#include <Model.h>

wxBEGIN_EVENT_TABLE(csGLPanel, wxGLCanvas)
	EVT_SIZE(csGLPanel::OnSize)
	EVT_PAINT(csGLPanel::OnPaint)
wxEND_EVENT_TABLE()

csGLPanel::csGLPanel(Model* model, wxWindow *parent, const wxGLAttributes& canvasAttrs) : wxGLCanvas(parent, canvasAttrs), m_winHeight(0)
{
	SetWindowStyle(wxBORDER_SUNKEN);

	m_model = model;

	//SetColour("gray");
	wxGLContextAttrs ctxAttrs;

	ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 2).EndList();
	m_GLContext = new wxGLContext(this, NULL, &ctxAttrs);

	if (!m_GLContext->IsOK())
	{
		wxLogFatalError("This sample needs an OpenGL 3.2 capable driver.\nThe app will end now.");
		//wxMessageBox("This sample needs an OpenGL 3.2 capable driver.\nThe app will end now.", "OpenGL version error", wxOK | wxICON_INFORMATION, this);
		delete m_GLContext;
		m_GLContext = NULL;
	}
	else
	{
		wxLogDebug("OpenGL Core Profile 3.2 successfully set.");
	}

	model->addActionListener(this);
}

csGLPanel::~csGLPanel()
{
	if (m_GLContext)
		SetCurrent(*m_GLContext);

	if (m_GLContext)
	{
		delete m_GLContext;
		m_GLContext = NULL;
	}
}

bool csGLPanel::GLInit()
{
	if (!m_GLContext)
		return false;

	SetCurrent(*m_GLContext);

	int ret = OpenGLContext::Instance().init();
	if (ret == 1) {// first time init
		OpenGLContext::Instance().setGrid(500, 500, 50);
	}

	return true;
}

void csGLPanel::OnSize(wxSizeEvent& event)
{
	event.Skip();

	const wxSize size = event.GetSize() * GetContentScaleFactor();
	m_winHeight = size.y;

	OpenGLContext::Instance().resize(size.x, size.y);

	if (!GLInit())
		return;
	
	//if (!IsShownOnScreen())
	//	return;

	Refresh(false);
}

void csGLPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// This is a dummy, to avoid an endless succession of paint messages.
	// OnPaint handlers must always create a wxPaintDC.
	wxPaintDC dc(this);

	// Avoid painting when we have not yet a size
	if (m_winHeight < 1)
		return;

	// This should not be needed, while we have only one canvas
	SetCurrent(*m_GLContext);

	OpenGLContext::Instance().render();

	SwapBuffers();
	//wxLogDebug("buffers swapped");
}

void csGLPanel::updated()
{
	Refresh();
	Update();
}
