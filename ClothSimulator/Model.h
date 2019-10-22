#pragma once

#include "glincludes.h"

#include <vector>
#include <ModelListener.h>
#include "ClothMesh.h"
#include "wx/thread.h"
#include <wx\log.h>

class SimulationThread;

class Vector2
{
public:
	Vector2() :x(0), y(0) {}
	Vector2(float vx, float vy) : x(vx), y(vy) {}

public:
	float x, y;
};

class ClothShape
{
public:
	ClothShape() : m_mesh(nullptr) {}
	void addVertex(float x, float y) { m_vertices.push_back( Vector2(x, y)); }

	int getCount() { return m_vertices.size(); }
	auto getVertex(int i) { return m_vertices.at(i); }
	auto &getVertices() const { return m_vertices; }

	void simulate();

	ClothMesh* m_mesh;

private:
	std::vector<Vector2> m_vertices;
};

class Model
{
public:
	Model() : m_simulate(false), m_thread(nullptr) {}
	~Model();

	void createCloth(float x1, float y1, float x2, float y2, float segment_length = 10, float tensile_strength = 0.7);

	void addActionListener(ModelListener* listener) { m_listeners.push_back(listener); }
	void notifyListeners();

	void simulate();

	auto &getShapes() { return m_shapes; };
	bool m_simulate;

	void getNearestClothPoint(float x, float y, std::vector<Vector2*>& points);

private:
	std::vector< std::shared_ptr<ClothShape> > m_shapes;
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
