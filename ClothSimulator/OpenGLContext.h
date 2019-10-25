#pragma once

#include <OpenGLContext.h>
#include "ClothMesh.h"

#include <memory>
#include <algorithm>

class OpenGLContext {
public:
	static OpenGLContext& Instance() {
		// This line only runs once, thus creating the only instance in existence
		static std::unique_ptr<OpenGLContext> instance(new OpenGLContext);
		// dereferencing the variable here, saves the caller from having to use 
		// the arrow operator, and removes temptation to try and delete the 
		// returned instance.
		return *instance;
	}

	int init();

	// camera / view
	void resize(int width, int height);

	// grid
	void setGrid(float width, float depth, float major);
	// 2D
	// 3D
	ClothMesh* createCloth(std::vector<glm::vec2>& vertices, std::vector<Polygon2>&polygons, float segment_length, float tensile_strength);
	void reCreateCloth(ClothMesh* mesh, std::vector<glm::vec2>& vertices, std::vector<Polygon2>& polygons, float segment_length, float tensile_strength);

	void deleteCloth(ClothMesh* cloth) {
		m_rendered_objects.erase(std::remove(m_rendered_objects.begin(), m_rendered_objects.end(), cloth), m_rendered_objects.end());;
	};
	// model

	// render
	void render();

	void clearObjects();

	~OpenGLContext();

private:
	OpenGLContext() : m_initialized(false) {}

	// Note that the next two functions are not given bodies, thus any attempt 
	 // to call them implicitly will return as compiler errors. This prevents 
	 // accidental copying of the only instance of the class.
	OpenGLContext(const OpenGLContext& old) : m_initialized(false) {}; // disallow copy constructor
	const OpenGLContext& operator=(const OpenGLContext& old) {}; //disallow assignment operator

private:
	bool m_initialized;
	
	std::vector< csGL3DObject* > m_rendered_objects;
};
