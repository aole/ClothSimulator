#include <GL/glew.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "csGLRectangle.h"

void csGLRectangle::create(float x1, float y1, float z1, float x2, float y2, float z2)
{
	m_draw_mode = GL_TRIANGLE_STRIP;

	std::vector< glm::vec3 > vertices;
	std::vector< unsigned int > indices;
	// create ground vertices

	// rect - clockwize
	vertices.push_back(glm::vec3(glm::min(x1, x2), glm::min(y1, y2), glm::min(z1, z2)));
	vertices.push_back(glm::vec3(glm::max(x1, x2), glm::min(y1, y2), glm::min(z1, z2)));
	vertices.push_back(glm::vec3(glm::max(x1, x2), glm::max(y1, y2), glm::max(z1, z2)));
	vertices.push_back(glm::vec3(glm::min(x1, x2), glm::max(y1, y2), glm::max(z1, z2)));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);

	creategl(vertices, indices);

	vertices.clear();
	indices.clear();
}
