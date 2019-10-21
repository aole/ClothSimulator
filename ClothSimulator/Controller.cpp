#include "Controller.h"
#include "csApplication.h"

void Controller::mouseUp2D(float screenx, float screeny, float logicalx, float logicaly)
{
	m_lastx = logicalx;
	m_lasty = logicaly;

	if (m_mouse_left_down) {
		m_model->createCloth(m_anchorx, m_anchory, m_lastx, m_lasty, 10, .99f);
	}
	m_mouse_left_down = false;
}

void Controller::mouseDown2D(float screenx, float screeny, float logicalx, float logicaly)
{
	m_lastx = m_anchorx = logicalx;
	m_lasty = m_anchory = logicaly;
	m_mouse_left_down = true;
}

void Controller::mouseMove2D(float screenx, float screeny, float logicalx, float logicaly)
{
	m_lastx = logicalx;
	m_lasty = logicaly;

	if (m_mouse_left_down)
		for (View2D* v : m_2Dviews) {
			v->drawTemporaryRectangle(m_anchorx, m_anchory, m_lastx, m_lasty);
		}
}

void Controller::keyDown(int keyCode)
{
	switch (keyCode)
	{
	case WXK_SPACE:
		OnToggleSimulation(!m_simulate);
		break;
	default:
		wxLogDebug("Other key pressed");
		break;
	}
}

void Controller::OnToggleSimulation(bool simulate)
{
	m_simulate = simulate;
	wxGetApp().activateRenderLoop(simulate);
}

void Controller::simulate()
{
	m_model->simulate();
}
