#pragma once
#include <wx/wx.h>

class WindowTerminationEvent : public wxCommandEvent {
public:
	int window_id;
	WindowTerminationEvent(int id);
};