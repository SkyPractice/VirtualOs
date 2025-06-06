#include "StartMenu.h"
#include "../VirtualOs/VirtualOsApp.h"

StartMenu::StartMenu(wxWindow* parent, uint32_t width, uint32_t height,
	uint32_t x, uint32_t y, uint32_t border_radius): wxPanel(parent, wxID_ANY, wxPoint(x, y),
     wxSize(width, height), 
    wxTRANSPARENT_WINDOW), border_rad(border_radius){
    
    shutdown_bmp = wxImage("shutdown.png");
    shutdown_bmp.UseAlpha(true);

    restart_bmp = wxImage("restart.png");
    restart_bmp.UseAlpha(true);

    this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->Bind(wxEVT_PAINT, &StartMenu::onPaint, this);
    this->Bind(wxEVT_LEFT_DOWN, &StartMenu::onLeftButtonClick, this);

}

void StartMenu::onPaint(wxPaintEvent& evt){
	wxPaintDC dc(this);
	wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);

    if(ctx){
        int w, h;
        this->GetSize(&w, &h);

        ctx->SetBrush(wxBrush(wxColor(40, 40, 40)));
        ctx->DrawRoundedRectangle(
            0, 0, w, h, border_rad
        );
        ctx->DrawBitmap(shutdown_bmp, 10, h - (START_MENU_ICON_HEIGHT + 10), 
            START_MENU_ICON_WIDTH, START_MENU_ICON_HEIGHT);
        ctx->DrawBitmap(restart_bmp, 10, h - (START_MENU_ICON_HEIGHT + START_MENU_ICON_HEIGHT + 20), 
            START_MENU_ICON_WIDTH, START_MENU_ICON_HEIGHT);

        delete ctx;
    }
}

void StartMenu::onLeftButtonClick(wxMouseEvent& evt){
    wxPoint pos = evt.GetPosition();

    int h, w;
    this->GetSize(&w, &h);

    if(pos.x > 10 && pos.y > h - (START_MENU_ICON_HEIGHT + 10) &&
        pos.x < 10 + START_MENU_ICON_WIDTH && h - (START_MENU_ICON_HEIGHT + START_MENU_ICON_HEIGHT + 10)){
            exit(0);
        }
    else if(pos.x > 10 && pos.y > h - (START_MENU_ICON_HEIGHT + START_MENU_ICON_HEIGHT + 10) &&
        pos.x < 10 + START_MENU_ICON_WIDTH && h - (START_MENU_ICON_HEIGHT
         + START_MENU_ICON_HEIGHT + START_MENU_ICON_HEIGHT + 10)){

            #ifdef _WIN32
                STARTUPINFO si = {sizeof(si)};
                PROCESS_INFORMATION pi;
                    CreateProcess(
                        VirtualOsApp::my_argv[0],
                        NULL, NULL, NULL, FALSE, NULL , NULL, NULL, &si, &pi
                    );
            #endif

            
            exit(0);
        }

}
