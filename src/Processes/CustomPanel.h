#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

class CustomPanel : public wxPanel{
public:
    wxColour bk_clr = wxColour(255, 255, 255);
    wxColour border_clr = wxColour(0, 0, 0);
    uint32_t border_weight = 3;
    uint32_t border_radius = 15;

    CustomPanel(wxWindow* p, wxPoint pos, wxSize size):
        wxPanel(p, wxID_ANY, pos, size, wxTRANSPARENT_WINDOW) {
        this->SetBackgroundStyle(wxBG_STYLE_PAINT);
        this->Bind(wxEVT_PAINT, &CustomPanel::onPaint, this); 
    };  

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
            int x, y, w, h;
            this->GetSize(&w, &h);
            this->GetSize(&x, &y);

            ctx->SetBrush(wxBrush(bk_clr));
            ctx->SetPen(wxPen(border_clr, border_weight));

            ctx->DrawRoundedRectangle(0 + border_weight, 0 + border_weight, w - (border_weight * 2), h - (border_weight * 2), border_radius);

            delete ctx;
        }
    }
};