#pragma once

#include "glincludes.h"

class World
{
private:
	// horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;

	int Viewport[4];

	glm::mat4 Projection;
	glm::mat4 Model = glm::mat4(1.0f);

	// Camera matrix
	glm::mat4 View;
	glm::vec3 CameraPosition = glm::vec3(0, 150, 700);
	glm::vec3 CameraLookAt = glm::vec3(0, 0, 0);
	glm::vec3 CameraDirection = glm::vec3(0, 0, 0);
	glm::vec3 CameraRight = glm::vec3(1, 0, 0);
	glm::vec3 CameraUp = glm::vec3(0, 1, 0);

	glm::vec3 Light = glm::vec3(200, 50, 200);

private:
	void update_matrices();

public:
	World();

	void resize_window(int width, int height);
	void unproject(int x, int y, double* ux, double* uy, double* uz);

	const GLfloat* get_projection_matrix_address() { return &Projection[0][0]; }
	const GLfloat* get_view_matrix_address() { return &View[0][0]; }
	const GLfloat* get_model_matrix_address() { return &Model[0][0]; }
	const GLfloat* get_light_vector_address() { return &Light[0]; }
};

