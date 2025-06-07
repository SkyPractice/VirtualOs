#pragma once
#include <wx/wx.h>
#include <string>
#include "../Processes/Process.h"
#include <mutex>


class WindowCreationEvent : public wxCommandEvent {
public:
	std::string window_name;
	Process* process_caller;
	std::vector<Interrupt>* interrupts_vec;
	std::mutex* kernel_mut;
	wxBitmap process_img;
	WindowType type;
	WindowCreationEvent(std::string name, Process* process, std::vector<Interrupt>* int_vec,
		std::mutex* kernel_mutex, wxBitmap img, WindowType t = ConsoleWindow);
};