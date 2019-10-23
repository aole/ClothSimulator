#include "Model.h"
#include <algorithm>
#include "wx/wx.h"
#include <OpenGLContext.h>
#include <glm/gtx/closest_point.hpp>

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

	shape->addVertex(maxx, maxy, true);
	shape->addVertex(minx, maxy, true);
	shape->addVertex(minx, miny);
	shape->addVertex(maxx, miny);

	m_shapes.push_back(shape);

	std::vector<glm::vec2> vertices;
	std::vector<int> pins;

	for (auto v : shape->getPoints()) {
		vertices.push_back(glm::vec2(v->x, v->y));
	}

	shape->m_mesh = OpenGLContext::Instance().createCloth(vertices, segment_length, tensile_strength);

	for (int i = 0;i < shape->getCount();i++) {
		if(shape->getPoint(i)->pin)
			shape->m_mesh->setPin(i, true);
	}

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

	for (int i = 0;i < shape->getCount();i++) {
		if (shape->getPoint(i)->pin)
			shape->m_mesh->setPin(i, true);
	}

	notifyListeners();
}

Vector2* Model::addClothPoint(Vector2IP& ip)
{
	return ip.parent->insertVertex(ip.x, ip.y, ip.insert_at);
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

float Model::getNearestEdgePoint(float x, float y, Vector2IP* mip)
{
	assert(mip);

	float min = 999999999.f; // std::numeric_limits<float>::max();
	glm::vec2 mp(x, y);

	for (auto s : m_shapes)
	{
		auto vl = s->getLast();
		int idx = 0;
		for (auto v : s->getPoints())
		{
			glm::vec2 ip = glm::closestPointOnLine(mp, *vl, *v);
			float d = glm::distance(mp, ip);

			if (d < min) {
				min = d;
				mip->set(ip.x, ip.y, s, idx);
			}

			vl = v;
			idx++;
		}
	}

	return min;
}

void Vector2::setPin(bool p)
{
	pin = p; parent->setPin(this, p);
}
