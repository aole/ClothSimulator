#pragma once

class ViewListener {
public:
	virtual void leftMouseUp2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
	virtual void leftMouseDown2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
	virtual void middleMouseUp2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
	virtual void middleMouseDown2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
	virtual void mouseMove2D(float screenx, float screeny, float logicalx, float logicaly) = 0;
};
