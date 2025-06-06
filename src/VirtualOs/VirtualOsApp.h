#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <wx/wx.h>
#include "../OsFrame/OsFrame.h"
#include "VirtualOs.h"
#include <thread>

class VirtualOsApp : public wxApp {
public:
	static wxChar** my_argv;
	bool OnInit() override;
};