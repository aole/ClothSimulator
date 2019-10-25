#pragma once

#include "glincludes.h"
#include <vector>

class Vertex : public glm::vec3
{
public:
	Vertex(glm::vec2 &v) : Vertex(v.x, v.y, 0) {}
	Vertex(glm::vec3 &v) : Vertex(v.x, v.y, v.z) {}
	Vertex(float x, float y, float z, bool pinned = false) : glm::vec3(x, y, z), m_previous(x, y, z), m_pinned(pinned), m_face_count(0) {}

	void sync() { m_previous.x = x; m_previous.y = y; m_previous.z = z; }

	bool m_pinned;
	glm::vec3 m_previous;
	int m_face_count;
};

class csGL3DObject {
public:
	virtual ~csGL3DObject();

	void render();
	virtual void get_color(float color[]) { color[0] = color[1] = color[2] = .5f; };

	virtual enum SHADER_TYPE get_shader_type() { return SHADER_LIT; }

	// SIMULATION
	virtual void addForce(glm::vec3 f) {}
	virtual void update() {}

protected:
	void creategl(std::vector< Vertex* >& vertices, std::vector< unsigned int >& indices, GLenum usage = GL_DYNAMIC_DRAW);
	void creategl(std::vector< glm::vec3 >& vertices, std::vector< unsigned int >& indices, GLenum usage = GL_STATIC_DRAW);

	void creategl(std::vector< Vertex* >& vertices, std::vector< glm::vec3* >& normals, std::vector< unsigned int >& indices, GLenum usage = GL_STATIC_DRAW);

	void updategl(std::vector< Vertex* >& vertices);
	void updategl(std::vector< glm::vec3 >& vertices);

	void updategl(std::vector< Vertex* >& vertices, std::vector< glm::vec3* >& normals);

	GLenum m_draw_mode;

private:
	GLsizei m_draw_count;
	GLuint m_vertexbuffer;
	GLuint m_elementbuffer;
	GLuint m_VertexArrayID;

	int m_vertex_attributes_used = 1;
};
