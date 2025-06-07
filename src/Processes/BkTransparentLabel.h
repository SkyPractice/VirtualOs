#pragma once 
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

class BkTransparentLabel : public wxStaticText {
public:
    BkTransparentLabel(wxWindow* p, int x, int y, int w, int h, std::string str, int font_size):
        wxStaticText(p, wxID_ANY, str, wxPoint(x, y), wxSize(w, h), wxTRANSPARENT_WINDOW) {
            this->SetBackgroundStyle(wxBG_STYLE_PAINT);
            this->Bind(wxEVT_PAINT, &BkTransparentLabel::onPaint, this);

        }

    void onPaint(wxPaintEvent& evt){
        wxPaintDC dc(this);
        wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);

        if(ctx){
            wxGraphicsFont fnt = ctx->CreateFont(this->GetFont());
            ctx->SetFont(fnt);

            ctx->DrawText(GetLabelText(), 0, 0);

            delete ctx;
        }

    };
};