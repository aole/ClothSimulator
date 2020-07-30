#pragma once

#include <Model.h>
#include <Controller.h>

extern bool render_wireframe;

class csApplication : public wxApp
{
public:
	csApplication() : render_loop_on(false) {}
	~csApplication();

	void activateRenderLoop(bool on);

private:
	bool OnInit() override;
	void OnIdle(wxIdleEvent& evt);
	
	bool render_loop_on;

	Model* m_model = nullptr;
	Controller* m_controller = nullptr;
};

DECLARE_APP(csApplication);
