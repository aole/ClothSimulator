#pragma once

#include <ViewListener.h>
#include <Model.h>
#include <View.h>

class Controller : public ViewListener
{
public:
	Controller(Model* model) : m_model(model) {}

	void addView(View* view) { view->addViewListener(this); }

	void rectangleAdded(float x1, float y1, float x2, float y2) { m_model->addRectangle(x1, y1, x2, y2); }

private:
	Model* m_model;
};
