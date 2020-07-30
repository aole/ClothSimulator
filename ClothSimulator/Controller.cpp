#include "Controller.h"
#include "csApplication.h"

#include <wx/textfile.h>

bool render_wireframe = false;

void Controller::leftMouseUp2D(float, float, float logicalx, float logicaly)
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

void Controller::leftMouseDown2D(float, float, float logicalx, float logicaly)
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
	else if (!intersected) {
		m_selected_shape = m_model->getShapeUnderPoint(logicalx, logicaly);
	}

	for (auto v : m_2Dviews) {
		v->setSelectedPoints(m_selected);
		v->setHighlightedPoints(m_highlighted);
	}

	m_mouse_left_down = true;
}

void Controller::middleMouseUp2D(float, float, float, float)
{
	m_mouse_middle_down = false;
}

void Controller::middleMouseDown2D(float screenx, float screeny, float logicalx, float logicaly)
{
	m_lastx = screenx;
	m_lasty = screeny;

	m_mouse_middle_down = true;
}

void Controller::mouseMove2D(float screenx, float screeny, float logicalx, float logicaly)
{
	if (m_mouse_left_down) {
		if (m_selected.empty() && !wxGetKeyState(WXK_CONTROL)) {
			if (m_selected_shape) {
				m_selected_shape->translateShape(logicalx - m_lastx, logicaly - m_lasty);
				for (auto v : m_2Dviews)
					v->update();
			}
			else {
				m_creating_rect = true;
				// new cloth
				for (auto v : m_2Dviews) {
					v->drawTemporaryRectangle(m_anchorx, m_anchory, logicalx, logicaly);
				}
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

		m_lastx = logicalx;
		m_lasty = logicaly;
	}
	else if (m_mouse_middle_down) {
		for (auto v : m_2Dviews)
			v->pan(screenx - m_lastx, screeny - m_lasty);

		m_lastx = screenx;
		m_lasty = screeny;
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
}

void Controller::On2DFileDropped(wxString filename)
{
	wxLogDebug("File '%s' dropped on 2D Panel.", filename);
	m_image = wxImage(filename);

	for (auto v : m_2Dviews) {
		v->setImage(m_image);
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
		break;
	}
}

void Controller::OnMenuFileNew(wxFrame* frame)
{
	for (auto v : m_2Dviews) {
		v->hideImage();
	}
	m_model->resetAll();
	m_model->notifyListeners();
}

void Controller::OnMenuFileAddObject(wxFrame* frame)
{
	wxLogDebug("OnMenuFileAddObject");
	wxTextFile objfile("Objects/FlagPole.obj");
	bool success = objfile.Open();
	wxLogDebug("opened: %i", success);

	for (wxString str = objfile.GetFirstLine(); !objfile.Eof(); str = objfile.GetNextLine())
	{
		if (str.StartsWith("o ")) { // new object
			wxString name = str.SubString(2, 0);
			wxLogDebug(name);
		}
	}

	objfile.Close();
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

void Controller::OnWireframeToggle(bool show)
{
	render_wireframe = show;
	m_model->notifyListeners();
}

void Controller::simulate()
{
	m_model->simulate();
}
