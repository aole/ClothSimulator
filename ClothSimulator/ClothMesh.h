#pragma once

#include "glincludes.h"
#include "csGL3DObject.h"

class Face
{
public:
	Face() : normal(glm::vec3()) {}
	Face(int a, int b, int c, int d) : normal(glm::vec3()) { indices.push_back(a); indices.push_back(b); indices.push_back(c); indices.push_back(d); }
	std::vector<int> indices;
	glm::vec3 normal;
};

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
	ClothMesh() : m_acceleration(glm::vec3()), m_segment_length(0), m_tensile_strength(0) {}
	virtual ~ClothMesh();

	virtual void get_color(float color[]) { color[0] = .8f; color[1] = color[2] = .3f; };

	void create(std::vector<glm::vec2> &vertices, float segment_length, float tensile_strength);
	void reCreate(std::vector<glm::vec2>& vertices, float segment_length, float tensile_strength);

	// SIMULATION
	void addForce(glm::vec3 f) { m_acceleration += f; }
	void constraint();
	void update();

	float m_segment_length;
	float m_tensile_strength;

private:
	void createLink(int v1, int v2);
	void clean();

	void updateNormals();

	std::vector<Vertex*> m_vertices;
	std::vector<glm::vec3*> m_normals;
	std::vector<Link*>m_links;
	std::vector<Face*>m_faces;

	glm::vec3 m_acceleration;
};
