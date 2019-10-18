#pragma once

#include "glincludes.h"
#include "csGL3DObject.h"

class Link
{
public:
	Link(int v1, int v2, float length) :m_v1(v1), m_v2(v2), m_length(length) {}

	int m_v1, m_v2;
	float m_length;
};

class ClothMesh : public csGL3DObject
{
public:
	virtual ~ClothMesh();

	virtual void get_color(float color[]) { color[0] = color[1] = color[2] = .3f; };
	void create(float x1, float y1, float x2, float y2, float z, float segment);

	// SIMULATION
	void addForce(glm::vec3 f) { m_acceleration += f; }
	void update();

private:
	void createLink(int v1, int v2);

	std::vector<Vertex*> m_vertices;
	std::vector<Link*>m_links;

	glm::vec3 m_acceleration;
};
