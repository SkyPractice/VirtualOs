#include "VirtualOsApp.h"
#include "../Kernel/Kernel.h"

wxChar** VirtualOsApp::my_argv = nullptr;

bool VirtualOsApp::OnInit() {

	OsFrame* frame = new OsFrame("VirtualOs", wxSize(600, 600));
	wxInitAllImageHandlers();


	VirtualOs* virtual_os = new VirtualOs(new DeskTop(frame));

	my_argv = argv;
	
	frame->ShowFullScreen(true);
	frame->Show();
	return true;
}