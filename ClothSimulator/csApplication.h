#pragma once

#include "wx/wx.h"

#include "csMainFrame.h"

class csApplication : public wxApp
{
public:
	bool OnInit();

public:
	csMainFrame *mainFrame = nullptr;
};
