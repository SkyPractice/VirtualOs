#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

class CustomImage : public wxPanel {
public:
    uint32_t border_rad = 15;
    wxBitmap bmp;
    CustomImage(wxWindow* p, wxBitmap bmp_arg, wxPoint pos, wxSize size):
        wxPanel(p, wxID_ANY, pos, size, wxTRANSPARENT_WINDOW), bmp(bmp_arg) {
        this->SetBackgroundStyle(wxBG_STYLE_PAINT);
        this->Bind(wxEVT_PAINT, &CustomImage::onPaint, this);       
    };

    void onPaint(wxPaintEvent& evt){
        wxPaintDC dc(this);
        wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);

        if(ctx){
            int w, h;
            this->GetSize(&w, &h);
            wxBitmap clipping_bmp(w, h, 32);
            wxMemoryDC clipping_dc(clipping_bmp);
            wxGraphicsContext* clip_ctx = wxGraphicsContext::Create(clipping_dc);
            if(clip_ctx){
                clip_ctx->SetBrush(*wxBLACK_BRUSH);
                clip_ctx->DrawRectangle(0, 0, w, h);
                wxGraphicsPath clip_path = clip_ctx->CreatePath();
                clip_ctx->SetBrush(*wxWHITE_BRUSH);
                clip_path.AddRoundedRectangle(0, 0, w, h, border_rad);
                clip_ctx->FillPath(clip_path);
                delete clip_ctx;
            };
            ctx->Clip(wxRegion(clipping_bmp, wxColour(0, 0, 0)));
            ctx->DrawBitmap(bmp, 0, 0, w, h);
            ctx->ResetClip();
            delete ctx;
        }
    }
};