#pragma once

#include <glm/vec3.hpp>
#include <vector>

class csGL3DObject {
public:
	void render();

protected:
	void creategl(std::vector< glm::vec3 >& vertices, std::vector< unsigned int >& indices);

	GLenum m_draw_mode;

private:
	GLsizei m_draw_count;
	GLuint m_vertexbuffer;
	GLuint m_elementbuffer;
	GLuint m_VertexArrayID;
};
