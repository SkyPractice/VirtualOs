#include "OsFrame.h"

OsFrame::OsFrame(std::string name, wxSize window_size) : wxFrame(nullptr, wxID_ANY, name, wxDefaultPosition, window_size) {
	srand(time(0));

}