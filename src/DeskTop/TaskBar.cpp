#include "TaskBar.h"
#include <cstdlib>

TaskBar::TaskBar(
	wxWindow* p,
	int height,
	StartMenu* startMenu
) : wxPanel(p, wxID_ANY, wxDefaultPosition, wxSize(500, height)),
	start_menu(startMenu) {

	wxImage img;
	img.LoadFile("os.png", wxBITMAP_TYPE_PNG);

	os_png = img;
	os_png.UseAlpha(true);

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &TaskBar::onPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent& evt) {
		if(evt.GetPosition().x < 32){
			start_menu->Show(!start_menu->IsShown());
		}
		});

};

void TaskBar::onPaint(wxPaintEvent& evt) {
	wxAutoBufferedPaintDC dc(this);
	wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);

	if (ctx) {
		int x, y, w, h;
		this->GetSize(&w, &h);
		this->GetPosition(&x, &y);

		ctx->SetBrush(wxBrush(wxColor(40, 40, 40)));
		ctx->DrawRectangle(0, 0, w, h);

		ctx->DrawBitmap(os_png, 5, 2, 30, 30);

		ctx->SetBrush(*wxWHITE_BRUSH);
		ctx->DrawRectangle(40, 0, 250, h);

		wxFont srch_fnt(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT);
		wxGraphicsFont srch_font = ctx->CreateFont(srch_fnt);
		ctx->SetFont(srch_font);
		wxDouble s_width, s_height;
		ctx->GetTextExtent("Search", &s_width, &s_height);
		ctx->DrawText("Search", 55, (h / 2) - (s_height / 2));
		if (processes) {
			for (int i = 0; i < processes->size(); i++) {
				ctx->DrawBitmap((*processes)[i]->window->process_bmp, 310 + (i * 40), 0, 30, 30);
			}
		}

		delete ctx;
	}


}
