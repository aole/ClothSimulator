#pragma once

#include "glincludes.h"

#include <vector>
#include <ModelListener.h>

class Shape
{
public:
	void addVertex(float x, float y);

	int getCount() { return m_vertices.size(); }
	glm::vec2 getVertex(int i) { return m_vertices.at(i); }
	std::vector<glm::vec2> getVertices() { return m_vertices; }

private:
	std::vector<glm::vec2> m_vertices;
};

class Model
{
public:
	~Model();

	void addRectangle(float x1, float y1, float x2, float y2);
	std::vector< Shape* > getShapes() { return m_shapes; };

	void addActionListener(ModelListener* listener) { m_listeners.push_back(listener); }

private:
	std::vector< Shape* > m_shapes;
	std::vector< ModelListener* > m_listeners;
};