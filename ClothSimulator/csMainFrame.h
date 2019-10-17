#pragma once

#include <View.h>
#include <ViewListener.h>
#include <Model.h>
#include <Controller.h>

#include <cs2DPanel.h>
#include <csGLPanel.h>

class csMainFrame : public wxFrame, public View
{
public:
	csMainFrame(Model* model, Controller* controller);

public:
	void CreateMenuBar();
	void CreatePanels();

	void addViewListener(ViewListener* l);

public:
	void OnExit(wxCommandEvent& event);

private:
	Model* m_model;
	Controller* m_controller;

	cs2DPanel* m_2DView;
	csGLPanel* m_3DView;
};
