#include "Model.h"
#include <algorithm>
#include "wx/wx.h"
#include <OpenGLContext.h>

const glm::vec3 GRAVITY(0, -0.01, 0);

ClothShape::~ClothShape()
{
	for (auto v : m_points)
		delete v;
}

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
	for (auto s : m_shapes)
		delete s;

	m_listeners.clear();
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

	std::vector<glm::vec2> vertices;
	for (auto v : shape->getPoints())
		vertices.push_back(glm::vec2(v->x, v->y));

	shape->m_mesh = OpenGLContext::Instance().createCloth(vertices, segment_length, tensile_strength);

	notifyListeners();
}

void Model::recreateCloth(ClothShape* shape)
{
	float segment_length = shape->m_mesh->m_segment_length;
	float tensile_strength = shape->m_mesh->m_tensile_strength;

	// delete shape->m_mesh;

	std::vector<glm::vec2> vertices;
	for (auto v : shape->getPoints())
		vertices.push_back(*v);

	OpenGLContext::Instance().reCreateCloth(shape->m_mesh, vertices, segment_length, tensile_strength);

	notifyListeners();
}

void Model::resetClothes()
{
	for (auto s : m_shapes)
		recreateCloth(s);
}

void Model::notifyListeners()
{
	for (ModelListener* l : m_listeners)
		l->updated();
}

void Model::simulate()
{
	for (auto s : m_shapes)
		s->simulate();

	notifyListeners();
}

float Model::getNearestClothPoint(float x, float y, std::vector<Vector2*>& points)
{
	float min = 999999999.f; // std::numeric_limits<float>::max();
	Vector2* minv = nullptr;

	for (auto s : m_shapes)
	{
		for (auto v : s->getPoints())
		{
			float d = v->distance(x, y);
			if (d < min) {
				min = d;
				minv = v;
			}
		}
	}

	if (minv) {
		points.push_back(minv);
		return min;
	}
	else
		return -1;
}
