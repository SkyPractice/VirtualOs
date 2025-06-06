
#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <vector>
#include "../Kernel/Interrupt.h"
#include <mutex>
#include "OutputLabel.h"
class Process;


enum WindowType {

	ConsoleWindow, GUIWindow

};



class ProcessWindow : public wxPanel {

public:

	std::string application_name;

	WindowType type;
	bool getting_dragged = false;
	wxPoint starting_pos;
	
	int id;
	wxSize last_size;
	wxPoint last_pos;
	std::vector<Interrupt>* interrupt_vec;

	std::mutex* kernel_mutex;
	Process* process;
	OutputLabel* output_label;
	wxBitmap process_bmp;

	ProcessWindow(wxWindow* parent, std::string app_name,
		std::vector<Interrupt>* interrupts, std::mutex* kernel_mut, Process* proc,
		wxBitmap img,
		WindowType t = ConsoleWindow, wxSize size = wxSize(400, 400));

	void onPaint(wxPaintEvent& evt);

	void onLeftButtonDown(wxMouseEvent& evt);
	void onLeftButtonUp(wxMouseEvent& evt);
	void onMotion(wxMouseEvent& evt);

};