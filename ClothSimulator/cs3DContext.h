#pragma once

#include <csGL3DObject.h>
#include "ClothMesh.h"

class cs3DContext {
public:
	cs3DContext();
	~cs3DContext();

	int init();

	// camera / view
	void resize(int width, int height);

	// grid
	void setGrid(float width, float depth, float major);
	// 2D
	// 3D
	ClothMesh* createCloth(float x1, float y1, float x2, float y2, float z, float segment_length, float tensile_strength);

	// model

	// render
	void render();

	void clearObjects();

private:
	bool m_initialized;
	
	GLint m_programID;

	std::vector< csGL3DObject* > m_rendered_objects;
};
