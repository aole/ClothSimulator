#pragma once

class ViewListener {
public:
	virtual void mouseUp2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
	virtual void mouseDown2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
	virtual void mouseMove2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
};
