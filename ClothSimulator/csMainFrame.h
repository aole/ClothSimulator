#pragma once

#include <View.h>
#include <ViewListener.h>
#include <Model.h>
#include <cs2DPanel.h>

class csMainFrame : public wxFrame, public View
{
public:
	csMainFrame(Model* model);

public:
	void CreateMenuBar();
	void CreatePanels();

	void addViewListener(ViewListener* l);

public:
	void OnExit(wxCommandEvent& event);

private:
	Model* m_model;

	cs2DPanel* m_2DView;
};
