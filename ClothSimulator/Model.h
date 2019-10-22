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
	Vector2(ClothShape* par) : Vector2(0, 0, par) {}
	Vector2(float vx, float vy, ClothShape* par) : glm::vec2(vx, vy), parent(par) {}

	float distance(float vx, float vy) { return sqrtf((x - vx) * (x - vx) + (y - vy) * (y - vy)); }

public:
	ClothShape* parent;
};

class ClothShape
{
public:
	ClothShape() : m_mesh(nullptr) {}
	~ClothShape();

	void addVertex(float x, float y) { m_points.push_back(new Vector2(x, y, this)); }

	int getCount() { return m_points.size(); }
	auto getPoint(int i) { return m_points.at(i); }
	auto &getPoints() const { return m_points; }

	void simulate();

	ClothMesh* m_mesh;

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
	void resetClothes();

	void addActionListener(ModelListener* listener) { m_listeners.push_back(listener); }
	void notifyListeners();

	void simulate();

	auto &getShapes() { return m_shapes; };
	bool m_simulate;

	float getNearestClothPoint(float x, float y, std::vector<Vector2*>& points);

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
