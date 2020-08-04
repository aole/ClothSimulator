#include "World.h"

World::World()
{
	update_matrices();
}

void World::update_matrices()
{
	// setup camera view
	CameraDirection = glm::vec3(std::cos(verticalAngle) * std::sin(horizontalAngle),
		std::sin(verticalAngle),
		std::cos(verticalAngle) * std::cos(horizontalAngle));

	CameraRight = glm::vec3(
		std::sin(horizontalAngle - 3.14f / 2.0f),
		0,
		std::cos(horizontalAngle - 3.14f / 2.0f)
	);
	CameraUp = glm::cross(CameraRight, CameraDirection);

	View = glm::lookAt(
		CameraPosition, // Camera position, in World Space
		CameraPosition + CameraDirection, // and looks at the origin
		CameraUp  // Head is up (set to 0,-1,0 to look upside-down)
	);

}

void World::resize_window(int width, int height)
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
	Viewport[2] = width;
	Viewport[3] = height;
	glViewport(0, 0, width, height);
	update_matrices();
}

void World::unproject(int x, int y, double* ux, double* uy, double* uz)
{
	//glm::vec4 pt = View * Projection * glm::vec4(x, y, 0, 1);
	glm::mat4 view = View * Model;
	glm::vec4 v(0, 0, Viewport[2], Viewport[3]);
	glm::vec3 pt = glm::project(glm::vec3(x, y, 0), view, Projection, v);
	*ux = pt.x; *uy = pt.y; *uz = pt.z;
}
