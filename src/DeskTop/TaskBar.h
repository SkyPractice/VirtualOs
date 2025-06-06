#pragma once
#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include "../Processes/Process.h"
#include "StartMenu.h"

class TaskBar : public wxPanel {
public:
	std::vector<Process*>* processes;
	wxBitmap os_png;
	StartMenu* start_menu;
	TaskBar(
		wxWindow* p,
		int height,
		StartMenu* startMenu
	);

	void onPaint(wxPaintEvent& evt);


};