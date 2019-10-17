#pragma once

#include "glincludes.h"
#include <vector>

class csGL3DObject {
public:
	void render();
	void get_color(float color[]);

protected:
	void creategl(std::vector< glm::vec3 >& vertices, std::vector< unsigned int >& indices);

	GLenum m_draw_mode;

private:
	GLsizei m_draw_count;
	GLuint m_vertexbuffer;
	GLuint m_elementbuffer;
	GLuint m_VertexArrayID;
};
