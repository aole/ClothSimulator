#pragma once

#include <ViewListener.h>
#include <Model.h>
#include <View.h>

#include "wx/wx.h"

class Controller : public ViewListener
{
public:
	Controller(Model* model) : m_model(model), m_mouse_left_down(false), m_simulate(false) {}

	void add2DView(View2D* view) { view->addViewListener(this); m_2Dviews.push_back(view); }

	void mouseUp2D(float screenx, float screeny, float logicalx, float logicaly);
	void mouseDown2D(float screenx, float screeny, float logicalx, float logicaly);
	void mouseMove2D(float screenx, float screeny, float logicalx, float logicaly);

	void OnMenuFileExit(wxFrame *frame) { frame->Close(true); };

	void OnToggleSimulation(bool simulate);
	void simulate();
private:
	Model* m_model;
	std::vector<View2D*> m_2Dviews;

	bool m_mouse_left_down;

	float m_lastx, m_lasty;
	float m_anchorx, m_anchory;

	bool m_simulate;
};
