#pragma once
#include <wx/wx.h>
#include "OsAppIcon.h"
#include <vector>
#include "../Kernel/Interrupt.h"
#include "../Processes/ProcessWindow.h"
#include <wx/event.h>
#include <algorithm>
#include "TaskBar.h"
#include "StartMenu.h"

class DeskTop : public wxPanel {

	std::vector<OsAppIcon*> icons;
	wxGridSizer* icon_sizer;

public:
	wxBitmap bmp;
	std::vector<Interrupt>* interrupt_vec;
	std::mutex* kernel_mut;
	TaskBar* task_bar;
	StartMenu* start_menu;	
	std::vector<ProcessWindow*> process_windows;

	DeskTop(wxWindow* parent);
	void addIcon(OsAppIcon* icon);
	void init();
	void onPaint(wxPaintEvent& evt);

};