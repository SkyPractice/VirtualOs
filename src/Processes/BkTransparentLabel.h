#pragma once 
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/setup.h>

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
            #ifdef __WXMSW__
            wxGraphicsRenderer* renderer = wxGraphicsRenderer::GetDirect2DRenderer();
        if (renderer)
        {
            delete ctx; // Delete the default context
            ctx = renderer->CreateContext(dc);
        }
        #endif
            ctx->SetAntialiasMode(wxANTIALIAS_DEFAULT);
            wxGraphicsFont fnt = ctx->CreateFont(this->GetFont(), this->GetForegroundColour());
            ctx->SetFont(fnt);
            ctx->DrawText(GetLabelText(), 0, 4);

            delete ctx;
        }

    };
};