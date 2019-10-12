#pragma once

#include <wx/glcanvas.h>

class csGLPanel : public wxGLCanvas
{
public:
	csGLPanel(wxWindow *parent);
	~csGLPanel();

	bool GLInit();

public:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);

private:
	wxGLContext *m_GLContext;

	int m_winHeight;

	bool m_glewInit;
	GLint m_programID;

	wxDECLARE_EVENT_TABLE();
};
