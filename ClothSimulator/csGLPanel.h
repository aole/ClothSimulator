#pragma once

#include "glincludes.h"

#include <wx/glcanvas.h>
#include <cs3DContext.h>
#include <View.h>
#include <ModelListener.h>
#include <Model.h>

class csGLPanel : public wxGLCanvas, public View, public ModelListener
{
public:
	csGLPanel(Model* model, wxWindow *parent, const wxGLAttributes& canvasAttrs);
	~csGLPanel();

	bool GLInit();

public:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);

	void updated();
private:
	Model* m_model;
	wxGLContext *m_GLContext;
	cs3DContext* m_3DContext;

	int m_winHeight;

	wxDECLARE_EVENT_TABLE();
};
