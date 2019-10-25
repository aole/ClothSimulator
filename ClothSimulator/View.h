#pragma once
#include <vector>
#include <ViewListener.h>
#include <Model.h>

#include "wx/wx.h"

class View
{
public:
	virtual void addViewListener(ViewListener* l) { m_listeners.push_back(l); }

protected:
	std::vector<ViewListener*> m_listeners;
};

class View2D : public View
{
public:
	virtual void drawTemporaryRectangle(float minx, float miny, float maxx, float maxy) = 0;
	virtual void setHighlightedPoints(std::vector<Vector2*>& points) = 0;
	virtual void setSelectedPoints(std::vector<Vector2*>& points) = 0;
	virtual void update() = 0;
	virtual void setImage(wxImage& image) = 0;
	virtual void hideImage() = 0;
	virtual void pan(float dx, float dy) = 0;
};
