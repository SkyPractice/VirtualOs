#include "ProcessWindow.h"
#include "Process.h"

ProcessWindow::ProcessWindow(wxWindow* parent, std::string app_name,
	std::vector<Interrupt>* interrupts, std::mutex* kernel_mut,
	Process* proc,
	wxBitmap img,
	WindowType t, wxSize size) : wxPanel(parent, wxID_ANY, wxPoint(50, 50), size, wxTRANSPARENT_WINDOW),
	application_name(app_name), type(t), interrupt_vec(interrupts), kernel_mutex(kernel_mut),
	process(proc), process_bmp(img){

	id = proc->random_iden;

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	int w, h;
	this->GetSize(&w, &h);
	last_size = this->GetSize();
	last_pos = this->GetPosition();
	output_label = new OutputLabel(this, w - 40, h - 30, 20, 50);
	proc->output_label = output_label;
	output_label->SetBackgroundColour(wxColour(0, 0, 0, 0));


	this->Bind(wxEVT_PAINT, &ProcessWindow::onPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, &ProcessWindow::onLeftButtonDown, this);
	this->Bind(wxEVT_LEFT_UP, &ProcessWindow::onLeftButtonUp, this);
	this->Bind(wxEVT_MOTION, &ProcessWindow::onMotion, this);

	Refresh();

};

void ProcessWindow::onPaint(wxPaintEvent& evt) {
	wxPaintDC dc(this);
	wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);

	if (ctx) {
		wxFont title_font(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		wxGraphicsFont g_font = ctx->CreateFont(title_font, wxColor(0, 0, 0));

		int x, y, w, h;

		this->GetPosition(&x, &y);
		this->GetSize(&w, &h);
		wxBitmap thing_bmp(w, h, 32);
		thing_bmp.UseAlpha(true);
		wxMemoryDC d(thing_bmp);
		wxGraphicsContext* img_ctx = wxGraphicsContext::Create(d);
		img_ctx->SetBrush(*wxBLACK_BRUSH);
		img_ctx->DrawRoundedRectangle(0, 0, w, h, 15);
		img_ctx->SetBrush(*wxWHITE_BRUSH);

		wxGraphicsPath path = ctx->CreatePath();
		// Start at bottom-left corner
		path.MoveToPoint(0, 40);

		// Bottom-left to bottom-right (straight line)
		path.AddLineToPoint(w, 40);

		// Bottom-right to top-right (straight line)
		path.AddLineToPoint(w, 15);

		// Top-right rounded corner
		path.AddArcToPoint(w, 0, w - 15, 0, 15);

		// Top edge (straight line)
		path.AddLineToPoint(15, 0);

		// Top-left rounded corner
		path.AddArcToPoint(0, 0, 0, 15, 15);

		// Left edge (straight line)
		path.AddLineToPoint(0, 40);

		img_ctx->FillPath(path);

		img_ctx->SetFont(g_font);
		img_ctx->DrawText(application_name, 10, 2);

		img_ctx->SetBrush(*wxRED_BRUSH);
		img_ctx->DrawEllipse(w - 40, 10, 20, 20);

		img_ctx->SetBrush(*wxGREEN_BRUSH);
		img_ctx->DrawEllipse(w - 70, 10, 20, 20);

		img_ctx->SetBrush(*wxYELLOW_BRUSH);
		img_ctx->DrawEllipse(w - 100, 10, 20, 20);

		ctx->DrawBitmap(thing_bmp, 0, 0, w, h);
		delete ctx;
	}
}


void ProcessWindow::onLeftButtonDown(wxMouseEvent& evt) {

	int x, y, w, h;

	this->GetPosition(&x, &y);
	this->GetSize(&w, &h);

	long mouse_x, mouse_y;

	evt.GetPosition(&mouse_x, &mouse_y);


	if (mouse_x > w - 40 && mouse_x < w - 20 && mouse_y < 30 && mouse_y > 10) {
		std::lock_guard<std::mutex> locker(*kernel_mutex);
		interrupt_vec->push_back({ UserModeInterrupt, ProcessTerminationInterrupt, this->process, nullptr });
	}
	else if (mouse_x > w - 70 && mouse_x < w - 50 && mouse_y < 30 && mouse_y > 10) {

		if (this->GetSize() != this->GetParent()->GetSize()) {
			last_pos = this->GetPosition();
			SetPosition(wxPoint(0, 0));
			last_size = this->GetSize();
			SetSize(this->GetParent()->GetSize());
		}
		else {
			this->SetSize(last_size);
			this->SetPosition(last_pos);
		}
		Refresh();
	}
	else if (mouse_y < 40) {
		getting_dragged = true;
		starting_pos = wxPoint(mouse_x, mouse_y);
		Refresh();
		CaptureMouse();
	}

}
void ProcessWindow::onLeftButtonUp(wxMouseEvent& evt) {
	getting_dragged = false;
	ReleaseMouse();
}void ProcessWindow::onMotion(wxMouseEvent& evt) {
	if (getting_dragged && evt.Dragging() && evt.LeftIsDown()) {
		wxPoint current_pos = evt.GetPosition();
		wxPoint screen_pos = ClientToScreen(current_pos);  // Convert to screen coordinates
		wxPoint new_pos(screen_pos.x - starting_pos.x, screen_pos.y - starting_pos.y);
		SetPosition(new_pos);
		Refresh();
	}

}

