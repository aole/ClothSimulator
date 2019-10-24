#pragma once

#include "glincludes.h"

#include <vector>
#include <ModelListener.h>
#include "ClothMesh.h"
#include "wx/thread.h"
#include <wx\log.h>

class SimulationThread;
class ClothShape;

class Vector2 : public glm::vec2
{
public:
	Vector2() : Vector2(nullptr) {}
	Vector2(ClothShape* par) : Vector2(0, 0, par) {}
	Vector2(float vx, float vy, ClothShape* par, bool p = false) : glm::vec2(vx, vy), parent(par), pin(p) {}
	void set(float vx, float vy, ClothShape* par = nullptr) { x = vx; y = vy; if (par) parent = par; }

	float distance(float vx, float vy) { return sqrtf((x - vx) * (x - vx) + (y - vy) * (y - vy)); }

	void setPin(bool p);

public:
	ClothShape* parent;
	bool pin;
};

class Vector2IP : public Vector2
{
public:
	Vector2IP() : Vector2(0, 0, nullptr), insert_at(-1) {}

	void set(float vx, float vy, ClothShape* par, int iat) { x = vx; y = vy; parent = par; insert_at = iat; }

public:
	int insert_at;
};

class Polygon2 {
public:
	Polygon2(int c = 0) { for (int i = 0;i < c;i++) indices.push_back(i); }

public:
	std::vector<int> indices;
};

void printpoly(Polygon2* p);

class ClothShape
{
public:
	ClothShape() : m_mesh(nullptr) {}
	~ClothShape();

	void addVertex(float x, float y, bool pin = false) { m_points.push_back(new Vector2(x, y, this, pin)); }
	Vector2* insertVertex(float x, float y, int insert_at) { Vector2* v = new Vector2(x, y, this); m_points.insert(m_points.begin()+insert_at, v); updateShape(); return v; }
	void updateShape();

	int getCount() { return m_points.size(); }
	auto getPoint(int i) { return m_points.at(i); }
	auto getLast() { return m_points.back(); }
	auto &getPoints() const { return m_points; }

	void simulate();
	void setPin(Vector2* v, bool pin) { m_mesh->setPin(std::find(m_points.begin(), m_points.end(), v)-m_points.begin(), pin); }

	ClothMesh* m_mesh;
	std::vector<Polygon2> m_polygons;

private:
	std::vector<Vector2*> m_points;
};

class Model
{
public:
	Model() : m_simulate(false), m_thread(nullptr) {}
	~Model();

	void createCloth(float x1, float y1, float x2, float y2, float segment_length = 10, float tensile_strength = 0.7);
	void recreateCloth(ClothShape* shape);
	Vector2* addClothPoint(Vector2IP& ip);
	void resetClothes();
	void resetAll();

	void addActionListener(ModelListener* listener) { m_listeners.push_back(listener); }
	void notifyListeners();

	void simulate();

	auto &getShapes() { return m_shapes; };
	bool m_simulate;

	float getNearestClothPoint(float x, float y, std::vector<Vector2*>& points);
	float getNearestEdgePoint(float x, float y, Vector2IP* points);

private:
	std::vector< ClothShape* > m_shapes;
	std::vector< ModelListener* > m_listeners;

	SimulationThread* m_thread;
};

class SimulationThread : public wxThread
{
public:
	SimulationThread(Model* model) : m_model(model) {}

private:
	ExitCode Entry() override {
		wxLogDebug("Thread Entry()");
		
		while (m_model->m_simulate) {
			//wxLogDebug("Simulating model!");
			for (auto s : m_model->getShapes())
				s->simulate();

			m_model->notifyListeners();
		}

		return 0;
	}

	Model* m_model;
};
