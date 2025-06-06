#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

class OutputLabel : public wxStaticText {
public:
    OutputLabel(wxWindow* p, int w, int h, int x, int y)
        : wxStaticText(p, wxID_ANY, "", wxPoint(x, y), wxSize(w, h)) {
        this->SetBackgroundStyle(wxBG_STYLE_PAINT);
        this->Bind(wxEVT_PAINT, &OutputLabel::onPaint, this);
    }

    void SetSmoothLabelText(const wxString& text) {
        if (text != GetLabelText()) {  // Update only if the text is different
            Freeze();  // Pause updates to prevent flicker
            SetLabelText(text);
            Refresh();  // Refresh only the necessary parts
            Thaw();  // Resume rendering smoothly
        }
    }

    void onPaint(wxPaintEvent& evt) {
        wxPaintDC dc(this);
        dc.Clear();
        wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);

        if (ctx) {
            int w, h;
            GetClientSize(&w, &h);

            ctx->SetBrush(*wxTRANSPARENT_BRUSH);
            ctx->SetPen(*wxTRANSPARENT_PEN);

            wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
            wxGraphicsFont fnt = ctx->CreateFont(font, wxColour(255, 255, 255));

            ctx->SetFont(fnt);
            ctx->DrawText(GetLabelText(), 5, 5);

            delete ctx;
        }
    }
};
