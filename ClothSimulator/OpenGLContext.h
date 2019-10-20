#pragma once

#include <OpenGLContext.h>
#include "ClothMesh.h"

#include <memory>

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
	ClothMesh* createCloth(float x1, float y1, float x2, float y2, float z, float segment_length, float tensile_strength);

	// model

	// render
	void render();

	void clearObjects();

	// Note that although this should be allowed, 
	// some compilers may not implement private destructors
	// This prevents others from deleting our one single instance, which was otherwise created on the heap
	~OpenGLContext();

private:
	OpenGLContext();

	// Note that the next two functions are not given bodies, thus any attempt 
	 // to call them implicitly will return as compiler errors. This prevents 
	 // accidental copying of the only instance of the class.
	OpenGLContext(const OpenGLContext& old) {}; // disallow copy constructor
	const OpenGLContext& operator=(const OpenGLContext& old) {}; //disallow assignment operator

private:
	bool m_initialized;
	
	GLint m_programID;

	std::vector< csGL3DObject* > m_rendered_objects;
};
