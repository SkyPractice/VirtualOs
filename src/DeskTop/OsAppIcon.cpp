#define _CRT_SECURE_NO_WARNINGS
#include "OsAppIcon.h"

wxSize OsAppIcon::icon_size = wxSize(60, 80);

OsAppIcon::OsAppIcon(wxWindow* parent,
	std::string icon_path, std::string app_source_path, std::string app_name_arg,
	std::vector<Interrupt>* interrupts_arg, std::mutex* mut) : wxPanel(parent, wxID_ANY,
		wxDefaultPosition,
		icon_size, wxTRANSPARENT_WINDOW), source_path(app_source_path), app_name(app_name_arg), interrupts(interrupts_arg),
		kernel_mut(mut){

	wxImage img;
	img.LoadFile(icon_path, wxBITMAP_TYPE_PNG);
	icon = img;

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &OsAppIcon::onDraw, this);
	this->Bind(wxEVT_LEFT_DOWN, &OsAppIcon::OnClick, this);
	this->Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& evt) {
		});
	this->SetCursor(wxCursor(wxCURSOR_HAND));
	Refresh();
}

void OsAppIcon::OnClick(wxMouseEvent& evt) {

	std::lock_guard<std::mutex> locker(*kernel_mut);
	interrupts->push_back({ UserModeInterrupt, ProcessCreation, nullptr, this });

	evt.Skip();
}

void OsAppIcon::onDraw(wxPaintEvent& evt) {

	wxPaintDC dc(this);
	wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);
	
	if (ctx) {
		 #ifdef __WXMSW__
            wxGraphicsRenderer* renderer = wxGraphicsRenderer::GetDirect2DRenderer();
        if (renderer)
        {
            delete ctx; // Delete the default context
            ctx = renderer->CreateContext(dc);
        }
        #endif
		int x, y, w, h;
		this->GetPosition(&x, &y);
		this->GetSize(&w, &h);

		wxFont fnt(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		wxGraphicsFont font = ctx->CreateFont(fnt, *wxWHITE);

		wxDouble txt_width, txt_height;
		
		ctx->SetFont(font);
		ctx->GetTextExtent(app_name, &txt_width, &txt_height);
		ctx->DrawText(app_name, (w / 2) - (txt_width / 2), 45);
		ctx->DrawBitmap(icon, 10, 10, icon_size.GetX() - 20, 40);
		delete ctx;
	}

}