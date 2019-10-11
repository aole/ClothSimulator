#include "csApplication.h"

wxIMPLEMENT_APP(csApplication);

bool csApplication::OnInit()
{
	mainFrame = new csMainFrame();
	mainFrame->Show();

	return true;
}
