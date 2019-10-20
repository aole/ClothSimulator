#include "Model.h"
#include <algorithm>
#include "wx/wx.h"
#include <OpenGLContext.h>

const glm::vec3 GRAVITY(0, -0.031, 0);

void ClothShape::simulate()
{
	if (m_mesh) {
		// GRAVITY
		m_mesh->addForce(GRAVITY);
		m_mesh->constraint();
		m_mesh->update();
	}
}

Model::~Model()
{
	m_listeners.clear();

	for (ClothShape* s : m_shapes)
		delete s;
	m_shapes.clear();
}

void Model::createCloth(float x1, float y1, float x2, float y2, float segment_length, float tensile_strength)
{
	ClothShape* shape = new ClothShape();
	// clockwize
	float minx = std::min(x1, x2);
	float miny = std::min(y1, y2);
	float maxx = std::max(x1, x2);
	float maxy = std::max(y1, y2);

	shape->addVertex(minx, miny);
	shape->addVertex(maxx, miny);
	shape->addVertex(maxx, maxy);
	shape->addVertex(minx, maxy);

	m_shapes.push_back(shape);

	shape->m_mesh = OpenGLContext::Instance().createCloth(minx, miny, maxx, maxy, 0, segment_length, tensile_strength);

	notifyListeners();
}

void Model::notifyListeners()
{
	for (ModelListener* l : m_listeners)
		l->updated();
}

void Model::simulate()
{
	for (ClothShape* s : getShapes())
		s->simulate();

	notifyListeners();
}
