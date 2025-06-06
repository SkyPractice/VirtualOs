#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <string>
#include <vector>
#include <cstdlib>
#ifdef _WIN32
	#include <Windows.h>
#endif

#define START_MENU_ICON_WIDTH 30
#define START_MENU_ICON_HEIGHT 30

enum StartMenuShortCutType {
	App, Icon
};

struct StartMenuShortCut {
	std::string name;
	std::string path;
	wxBitmap img;
};

class StartMenu : public wxPanel {
public:
	std::vector<StartMenuShortCut> short_cuts;
	uint32_t border_rad;
	wxBitmap shutdown_bmp;
	wxBitmap restart_bmp;

	StartMenu(wxWindow* parent, uint32_t width, uint32_t height,
		uint32_t x, uint32_t y, uint32_t border_radius);

	void onPaint(wxPaintEvent& evt);
	void onLeftButtonClick(wxMouseEvent& evt);


};