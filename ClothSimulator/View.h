#pragma once
#include <vector>
#include <ViewListener.h>

class View
{
public:
	virtual void addViewListener(ViewListener* l) { m_listeners.push_back(l); }

protected:
	std::vector<ViewListener*> m_listeners;
};