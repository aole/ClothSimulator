#pragma once

#include <glm/vec3.hpp>

class cs3DContext {
	void setView(glm::vec3 eye, glm::vec3 lookat);
	void line(glm::vec3 v1, glm::vec3);
};
