#pragma once

#include "csMainFrame.h"

class csApplication : public wxApp
{
public:
	bool OnInit();

public:
	csMainFrame *mainFrame = nullptr;
};
