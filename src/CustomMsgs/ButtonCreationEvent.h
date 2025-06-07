#pragma once
#include <wx/wx.h>
#include <string>
#include "../Processes/Process.h"
#include <mutex>
#include <condition_variable>


class ButtonCreationEvent : public wxCommandEvent {
public:
	std::string button_name;
    wxWindow* window;
    int x, y, w, h;
    std::condition_variable& condition_v;
    wxButton* btn;
    std::mutex& mtx;
	ButtonCreationEvent(std::string btn_name, wxWindow* win, int xpos, int ypos , int width, int height,
        wxButton* bt_var, std::condition_variable& v, std::mutex& mut):
        button_name(btn_name), window(win), x(xpos), y(ypos), w(width), h(height), condition_v(v), btn(bt_var), mtx(mut) {};
};