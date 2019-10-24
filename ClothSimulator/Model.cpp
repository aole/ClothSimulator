#define PI 3.14159265358979323846

#include "Model.h"
#include <algorithm>
#include "wx/wx.h"
#include <OpenGLContext.h>
#include <glm/gtx/closest_point.hpp>

const glm::vec3 GRAVITY(0, -0.01, 0);

float turn(Vector2* a, Vector2* b, Vector2* v) {
	float ab = atan2(b->y - a->y, b->x - a->x);
	float av = atan2(v->y - a->y, v->x - a->x);
	float d = av - ab;
	d = d < -PI? d + PI * 2 : d;
	d = d > PI ? d - PI * 2 : d;

	return d;
}

void printpoly(Polygon2* p) {
	wxString msg;
	msg << "    ";
	for (auto i : p->indices)
		msg << i << ",";
	wxLogDebug(msg);
}

void convexify(const std::vector<Vector2*> &points, std::vector<Polygon2> &ps) {
	wxLogDebug("convexify");

	std::vector<Polygon2> newps;
	for (auto poly : ps) {
		int first_idx = 0;
		int second_idx = 1;
		int start_idx = 0;
		int last_idx = 2;
		
		Polygon2* old_poly = new Polygon2();
		old_poly->indices.push_back(poly.indices[first_idx]);
		old_poly->indices.push_back(poly.indices[second_idx]);
		Polygon2* new_poly = new Polygon2();
		Polygon2* cur_poly = old_poly;

		bool turned = false;
		while (last_idx != start_idx) {
			Vector2* v0 = points[poly.indices[first_idx]];
			Vector2* v1 = points[poly.indices[second_idx]];
			Vector2* v2 = points[poly.indices[last_idx]];

			float t = turn(v0, v1, v2);
			if (t < 0 && !turned) { // took a right turn
				wxLogDebug("  turning on %i, %i", second_idx, last_idx);
				turned = true;
				cur_poly = cur_poly == old_poly ? new_poly : old_poly;
				cur_poly->indices.push_back(poly.indices[first_idx + 1]);
				cur_poly->indices.push_back(poly.indices[last_idx]);
			}
			else if (t >= 0 && turned) { // back on left turn
				wxLogDebug("  back on %i, %i", second_idx, last_idx);
				turned = false;
				cur_poly->indices.push_back(poly.indices[last_idx]);
				cur_poly = cur_poly == old_poly ? new_poly : old_poly;
				cur_poly->indices.push_back(poly.indices[last_idx]);
				newps.push_back(*new_poly);
				wxLogDebug("  add new poly1: %i", new_poly->indices.size());
				printpoly(new_poly);
				delete new_poly;
				new_poly = new Polygon2();
			}
			else if (turned) {
				cur_poly->indices.push_back(poly.indices[last_idx]);
			}
			else{
				cur_poly->indices.push_back(poly.indices[last_idx]);
				first_idx = second_idx;
				second_idx = last_idx;
			}
			last_idx++;
			last_idx %= poly.indices.size();
		}
		newps.push_back(*old_poly);
		wxLogDebug("  add old poly: %i", old_poly->indices.size());
		printpoly(old_poly);
		if (turned) {
			new_poly->indices.push_back(poly.indices[last_idx]);
			wxLogDebug("  add new poly2: %i", new_poly->indices.size());
			printpoly(new_poly);
			newps.push_back(*new_poly);
		}
		delete old_poly;
		delete new_poly;
	}
	ps = newps;
}

ClothShape::~ClothShape()
{
	for (auto v : m_points)
		delete v;
}

void ClothShape::updateShape()
{
	m_polygons.clear();
	// create concave polygon of all the vertices (clockwize)
	Polygon2 p(m_points.size());
	m_polygons.push_back(p);
	// split the polygon recursively until only a set of convex polygons are left
	convexify(m_points, m_polygons);
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

	shape->updateShape();

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

	shape->updateShape();

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
