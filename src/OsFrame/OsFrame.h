#pragma once
#include <wx/wx.h>
#include <string>
#include "../DeskTop/DeskTop.h"
#include <ctime>

class OsFrame : public wxFrame {
public:
	OsFrame(std::string name, wxSize window_size);
};