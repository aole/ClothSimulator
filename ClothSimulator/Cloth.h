#pragma once

#include "glincludes.h"
#include "csGL3DObject.h"

class Cloth : public csGL3DObject
{
public:
	void create(float x1, float y1, float x2, float y2, float z, float segment);
};
