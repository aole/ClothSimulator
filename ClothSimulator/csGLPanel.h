#pragma once

#include <wx/glcanvas.h>
#include <cs3DContext.h>

class csGLPanel : public wxGLCanvas
{
public:
	csGLPanel(wxWindow *parent, const wxGLAttributes& canvasAttrs);
	~csGLPanel();

	bool GLInit();

public:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);

private:
	wxGLContext *m_GLContext;
	cs3DContext* m_3DContext;

	int m_winHeight;

	wxDECLARE_EVENT_TABLE();
};
