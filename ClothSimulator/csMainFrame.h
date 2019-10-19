#pragma once

#include <View.h>
#include <ViewListener.h>
#include <Model.h>
#include <Controller.h>

#include <cs2DPanel.h>
#include <csGLPanel.h>

#include <wx/wx.h>

#include "csApplication.h"

class csMainFrame : public wxFrame, public View
{
public:
	csMainFrame(Model* model, Controller* controller);

public:
	void CreateMenuBar();
	void CreateToolStatusBar();
	void CreatePanels();

	void addViewListener(ViewListener* l);

	void OnClose(wxCloseEvent& evt)
	{
		wxGetApp().activateRenderLoop(false);
		evt.Skip(); // don't stop event, we still want window to close
	}

	wxDECLARE_EVENT_TABLE();

private:
	Model* m_model;
	Controller* m_controller;

	cs2DPanel* m_2DView;
	csGLPanel* m_3DView;
};
