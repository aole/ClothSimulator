#pragma once

class cs3DContext {
public:
	cs3DContext();

	void init();

	// camera / view
	void resize(int width, int height);

	// grid
	// 2D
	// 3D
	// model

	// render
	void render();

private:
	bool m_initialized;

	GLint m_programID;
};
