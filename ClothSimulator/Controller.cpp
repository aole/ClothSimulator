#include "Controller.h"
#include "csApplication.h"

void Controller::mouseUp2D(float, float, float logicalx, float logicaly)
{
	m_lastx = logicalx;
	m_lasty = logicaly;

	if (m_selected.empty()) {
		if (m_mouse_left_down) {
			m_model->createCloth(m_anchorx, m_anchory, m_lastx, m_lasty, 10, .99f);
		}
	}
	else {
		for (auto v : m_selected) {
			m_model->recreateCloth(v->parent);
		}
	}
	m_mouse_left_down = false;
}

void Controller::mouseDown2D(float, float, float logicalx, float logicaly)
{
	m_lastx = m_anchorx = logicalx;
	m_lasty = m_anchory = logicaly;

	// dragging points
	m_selected = m_highlighted;

	m_mouse_left_down = true;
}

void Controller::mouseMove2D(float, float, float logicalx, float logicaly)
{
	if (m_mouse_left_down) {
		if (m_selected.empty()) {
			// new cloth
			for (auto v : m_2Dviews) {
				v->drawTemporaryRectangle(m_anchorx, m_anchory, logicalx, logicaly);
			}
		}
		else {
			for (auto v : m_selected) {
				v->x += logicalx - m_lastx;
				v->y += logicaly - m_lasty;
				for (auto v : m_2Dviews)
					v->update();
			}
		}
	}
	else
		// cloth point highlight
	{
		m_highlighted.clear();
		float dist = m_model->getNearestClothPoint(logicalx, logicaly, m_highlighted);

		if (!m_highlighted.empty() && dist>5) {
			m_highlighted.pop_back();
		}
		for (auto v : m_2Dviews)
			v->setHighlightedPoints(m_highlighted);
	}

	m_lastx = logicalx;
	m_lasty = logicaly;
}

void Controller::keyDown(int keyCode)
{
	switch (keyCode)
	{
	case WXK_SPACE:
		OnToggleSimulation(!m_simulate);
		break;
	default:
		break;
	}
}

void Controller::OnToggleSimulation(bool simulate)
{
	m_simulate = simulate;
	wxGetApp().activateRenderLoop(simulate);
}

void Controller::OnRestCloth()
{
	OnToggleSimulation(false);
	m_model->resetClothes();
}

void Controller::simulate()
{
	m_model->simulate();
}
