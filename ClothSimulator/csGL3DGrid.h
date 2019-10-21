#pragma once

#include "csGL3DObject.h"

class csGL3DGrid : public csGL3DObject
{
public:
	void create(float width, float depth, float major);

	enum SHADER_TYPE get_shader_type() override { return SHADER_UNLIT; }
};
