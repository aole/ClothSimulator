#pragma once

#include "csMainFrame.h"
#include <Model.h>
#include <Controller.h>

class csApplication : public wxApp
{
public:
	~csApplication();

	bool OnInit();

private:
	csMainFrame* m_mainFrame = nullptr;
	Model* m_model = nullptr;
	Controller* m_controller = nullptr;
};
