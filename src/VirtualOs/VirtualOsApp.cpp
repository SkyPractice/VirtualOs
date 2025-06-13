#include "VirtualOsApp.h"
#include "../Kernel/Kernel.h"
#include <wx/rawbmp.h> // Sometimes needed for modern image handlers, or just wx/image.h
#include <wx/image.h>   // Required for wxImage and image handlers

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