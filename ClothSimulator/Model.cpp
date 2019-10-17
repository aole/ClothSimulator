#include "Model.h"
#include <algorithm>
#include "wx/wx.h"

void Shape::addVertex(float x, float y)
{
	m_vertices.push_back(glm::vec2(x, y));
}

Model::~Model()
{
	m_listeners.clear();

	for (Shape* s : m_shapes)
		delete s;
	m_shapes.clear();
}

void Model::addRectangle(float x1, float y1, float x2, float y2)
{
	Shape* shape = new Shape();
	// clockwize
	shape->addVertex(std::min(x1, x2), std::min(y1, y2));
	shape->addVertex(std::max(x1, x2), std::min(y1, y2));
	shape->addVertex(std::max(x1, x2), std::max(y1, y2));
	shape->addVertex(std::min(x1, x2), std::max(y1, y2));

	m_shapes.push_back(shape);

	for (ModelListener* l : m_listeners)
		l->updated();
}
