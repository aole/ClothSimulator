#pragma once

#include <ViewListener.h>
#include <Model.h>
#include <View.h>

#include "wx/wx.h"

class Controller : public ViewListener
{
public:
	Controller(Model* model) : m_model(model), m_mouse_left_down(false), m_simulate(false), m_lastx(0), m_lasty(0), m_anchorx(0), m_anchory(0){}

	void add2DView(View2D* view) { view->addViewListener(this); m_2Dviews.push_back(view); }

	// MOUSE INTERACTIONS
	void mouseUp2D(float screenx, float screeny, float logicalx, float logicaly);
	void mouseDown2D(float screenx, float screeny, float logicalx, float logicaly);
	void mouseMove2D(float screenx, float screeny, float logicalx, float logicaly);

	// KEYBOARD INTERACTIONS
	void keyDown(int keyCode);

	// MENU INTERACTIONS
	void OnMenuFileExit(wxFrame *frame) { frame->Close(true); };

	void OnToggleSimulation(bool simulate);
	void simulate();
private:
	Model* m_model;
	std::vector<View2D*> m_2Dviews;

	bool m_mouse_left_down;
	std::vector<Vector2*> m_highlighted;

	float m_lastx, m_lasty;
	float m_anchorx, m_anchory;

	bool m_simulate;
};
