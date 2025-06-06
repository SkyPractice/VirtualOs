#pragma once
#include <wx/wx.h>
#include <string>
#include <wx/image.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <vector>
#include "../Kernel/Interrupt.h"
#include <mutex>

class OsAppIcon : public wxPanel {

	static wxSize icon_size;
	std::vector<Interrupt>* interrupts;
	std::mutex* kernel_mut;

public:

	wxBitmap icon;
	std::string source_path;
	std::string app_name;
	wxStaticText* app_txt;

	OsAppIcon(wxWindow* parent,
		std::string icon_path, std::string app_source_path, std::string app_name_arg,
		std::vector<Interrupt>* interrupts_arg, std::mutex* in_mutex);
	void OnClick(wxMouseEvent& evt);
	void onDraw(wxPaintEvent& evt);

};