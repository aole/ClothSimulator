#pragma once

#include <Model.h>
#include <Controller.h>

class csApplication : public wxApp
{
public:
	~csApplication();

	void activateRenderLoop(bool on);

private:
	bool OnInit();
	void OnIdle(wxIdleEvent& evt);
	
	bool render_loop_on;

	Model* m_model = nullptr;
	Controller* m_controller = nullptr;
};

DECLARE_APP(csApplication);
