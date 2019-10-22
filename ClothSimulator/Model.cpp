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
	m_listeners.clear();
}

void Model::createCloth(float x1, float y1, float x2, float y2, float segment_length, float tensile_strength)
{
	std::shared_ptr<ClothShape> shape = std::make_shared<ClothShape>();

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
