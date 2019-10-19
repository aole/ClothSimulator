#pragma once

#include "glincludes.h"

#include <vector>
#include <ModelListener.h>
#include <cs3DContext.h>
#include "ClothMesh.h"
#include "wx/thread.h"
#include <wx\log.h>

class SimulationThread;

class ClothShape
{
public:
	void addVertex(float x, float y) { m_vertices.push_back(glm::vec2(x, y)); }

	int getCount() { return m_vertices.size(); }
	glm::vec2 getVertex(int i) { return m_vertices.at(i); }
	std::vector<glm::vec2> getVertices() { return m_vertices; }

	void simulate();

	ClothMesh* m_mesh;

private:
	std::vector<glm::vec2> m_vertices;
};

class Model
{
public:
	Model() : m_3DContext(nullptr), m_simulate(false), m_thread(nullptr) {}
	~Model();

	void set3DContext(cs3DContext* context) { m_3DContext = context; }

	void createCloth(float x1, float y1, float x2, float y2, float segment_length = 10, float tensile_strength = 0.7);

	void addActionListener(ModelListener* listener) { m_listeners.push_back(listener); }
	void notifyListeners();

	void simulate(bool simulate);

	std::vector< ClothShape* > getShapes() { return m_shapes; };
	bool m_simulate;
private:
	cs3DContext* m_3DContext;

	std::vector< ClothShape* > m_shapes;
	std::vector< ModelListener* > m_listeners;

	SimulationThread* m_thread;
};

class SimulationThread : public wxThread
{
public:
	SimulationThread(Model* model) : m_model(model) {}

private:
	ExitCode Entry() {
		wxLogDebug("Thread Entry()");
		
		while (m_model->m_simulate) {
			//wxLogDebug("Simulating model!");
			for (ClothShape* s : m_model->getShapes())
				s->simulate();

			m_model->notifyListeners();
		}

		return 0;
	}

	Model* m_model;
};
