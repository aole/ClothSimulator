#pragma once

#include <ViewListener.h>
#include <Model.h>
#include <View.h>

#include "wx/wx.h"

class Controller : public ViewListener
{
public:
	Controller(Model* model) : m_model(model), m_mouse_left_down(false), m_mouse_middle_down(false), m_creating_rect(false), m_simulate(false), m_lastx(0), m_lasty(0), m_anchorx(0), m_anchory(0), intersected(false) {}

	void add2DView(View2D* view) { view->addViewListener(this); m_2Dviews.push_back(view); }

	// MOUSE INTERACTIONS
	void leftMouseUp2D(float screenx, float screeny, float logicalx, float logicaly);
	void leftMouseDown2D(float screenx, float screeny, float logicalx, float logicaly);
	void middleMouseUp2D(float screenx, float screeny, float logicalx, float logicaly);
	void middleMouseDown2D(float screenx, float screeny, float logicalx, float logicaly);
	void mouseMove2D(float screenx, float screeny, float logicalx, float logicaly);

	void On2DFileDropped(wxString filename);

	// KEYBOARD INTERACTIONS
	void keyDown(int keyCode);

	// MENU INTERACTIONS
	void OnMenuFileNew(wxFrame* frame);
	void OnMenuFileAddObject(wxFrame* frame);
	void OnMenuFileExit(wxFrame *frame) { frame->Close(true); };

	void OnToggleSimulation(bool simulate);
	void OnRestCloth();
	void OnPinPoint();
	void OnWireframeToggle(bool show);

	void simulate();
private:
	Model* m_model;
	std::vector<View2D*> m_2Dviews;

	bool m_mouse_left_down;
	bool m_mouse_middle_down;
	bool m_creating_rect;

	std::vector<Vector2*> m_highlighted;
	std::vector<Vector2*> m_selected;
	Vector2IP m_intersection_point;
	bool intersected;

	float m_lastx, m_lasty;
	float m_anchorx, m_anchory;

	bool m_simulate;

	wxImage m_image;
};
