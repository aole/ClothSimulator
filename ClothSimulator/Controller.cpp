#include "Controller.h"
#include "csApplication.h"

void Controller::mouseUp2D(float, float, float logicalx, float logicaly)
{
	m_lastx = logicalx;
	m_lasty = logicaly;

	if (m_mouse_left_down) {
		if (m_selected.empty() && m_creating_rect) {
			m_model->createCloth(m_anchorx, m_anchory, m_lastx, m_lasty, 10, .99f);
		}
		else {
			for (auto v : m_selected) {
				m_model->recreateCloth(v->parent);
			}
		}
	}
	m_mouse_left_down = false;
	m_creating_rect = false;
}

void Controller::mouseDown2D(float, float, float logicalx, float logicaly)
{
	m_lastx = m_anchorx = logicalx;
	m_lasty = m_anchory = logicaly;

	// dragging points
	m_selected = m_highlighted;
	// remove the intersected point
	// add the intersected point to the shape
	if (intersected && wxGetKeyState(WXK_CONTROL)) {
		m_highlighted.clear();
		m_selected.clear();
		intersected = false;

		Vector2 *ip = m_model->addClothPoint(m_intersection_point);

		m_highlighted.push_back(ip);
		m_selected.push_back(ip);
		for (auto v : m_2Dviews)
			v->update();
	}

	for (auto v : m_2Dviews) {
		v->setSelectedPoints(m_selected);
		v->setHighlightedPoints(m_highlighted);
	}

	m_mouse_left_down = true;
}

void Controller::mouseMove2D(float, float, float logicalx, float logicaly)
{
	if (m_mouse_left_down) {
		if (m_selected.empty() && !wxGetKeyState(WXK_CONTROL)) {
			m_creating_rect = true;
			// new cloth
			for (auto v : m_2Dviews) {
				v->drawTemporaryRectangle(m_anchorx, m_anchory, logicalx, logicaly);
			}
		}
		if (!m_selected.empty()) {
			for (auto v : m_selected) {
				v->x += logicalx - m_lastx;
				v->y += logicaly - m_lasty;
				v->parent->updateShape();
				for (auto v : m_2Dviews)
					v->update();
			}
		}
	}
	else
		// cloth point highlight
	{
		m_highlighted.clear();
		intersected = false;
		float dist = m_model->getNearestClothPoint(logicalx, logicaly, m_highlighted);

		// remove from highlighted list if too far.
		if (!m_highlighted.empty() && dist>5) {
			m_highlighted.pop_back();
		}

		if (m_highlighted.empty()) {
			// show where the new point will be created
			// if ctrl is pressed
			if (wxGetKeyState(WXK_CONTROL)) {
				float dist = m_model->getNearestEdgePoint(logicalx, logicaly, &m_intersection_point);
				// remove from highlighted list if too far.
				if (dist < 5) {
					m_highlighted.push_back(&m_intersection_point);
					intersected = true;
				}
			}
		}

		for (auto v : m_2Dviews) {
			v->setSelectedPoints(m_selected);
			v->setHighlightedPoints(m_highlighted);
		}
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

void Controller::OnPinPoint()
{
	for (auto v : m_selected) {
		v->setPin(!v->pin);
	}
	m_model->notifyListeners();
}

void Controller::simulate()
{
	m_model->simulate();
}
