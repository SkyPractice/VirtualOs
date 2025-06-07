#include "ProcessWindow.h"
#include "Process.h"
#include "../CustomMsgs/ButtonCreationEvent.h"


wxDECLARE_EVENT(BUTTON_CREATION_EVENT, ButtonCreationEvent);
wxDEFINE_EVENT(BUTTON_CREATION_EVENT, ButtonCreationEvent);

ProcessWindow::ProcessWindow(wxWindow* parent, std::string app_name,
	std::vector<Interrupt>* interrupts, std::mutex* kernel_mut,
	Process* proc,
	wxBitmap img,
	WindowType t, wxSize size) : wxPanel(parent, wxID_ANY, wxPoint(50, 50), size),
	application_name(app_name), type(t), interrupt_vec(interrupts), kernel_mutex(kernel_mut),
	process(proc), process_bmp(img){

	id = proc->random_iden;

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	int w, h;
	this->GetSize(&w, &h);
	last_size = this->GetSize();
	last_pos = this->GetPosition();
	if (type == ConsoleWindow)
	{
		output_label = new OutputLabel(this, w - 40, h - 30, 20, 50);
		proc->output_label = output_label;
		output_label->SetBackgroundColour(wxColour(0, 0, 0, 0));
	}
	this->Bind(wxEVT_PAINT, &ProcessWindow::onPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, &ProcessWindow::onLeftButtonDown, this);
	this->Bind(wxEVT_LEFT_UP, &ProcessWindow::onLeftButtonUp, this);
	this->Bind(wxEVT_MOTION, &ProcessWindow::onMotion, this);
	this->Bind(BUTTON_CREATION_EVENT, [&](ButtonCreationEvent& evt){
		{
		std::unique_lock<std::mutex> lock(evt.mtx);
		evt.btn = new wxButton(evt.window, wxID_ANY, evt.button_name, 
							wxPoint(evt.x, evt.y), wxSize(evt.w, 
							evt.h));
		}
		evt.condition_v.notify_all();

	});

	Refresh();

};

void ProcessWindow::onPaint(wxPaintEvent& evt) {
	wxAutoBufferedPaintDC dc(this);
	wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);

	if (ctx) {
		wxFont title_font(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		wxGraphicsFont g_font = ctx->CreateFont(title_font, wxColor(0, 0, 0));

		int x, y, w, h;

		this->GetPosition(&x, &y);
		this->GetSize(&w, &h);

		if(type == ConsoleWindow){
			ctx->SetBrush(*wxBLACK_BRUSH);
		} else if (type == GUIWindow){
			ctx->SetBrush(wxColour(212, 208, 200));
		}
		ctx->DrawRectangle(0, 0, w, h);
		ctx->SetBrush(*wxWHITE_BRUSH);
		ctx->DrawRectangle(0, 0, w, 40);

		ctx->SetFont(g_font);
		ctx->DrawText(application_name, 10, 2);

		ctx->SetBrush(*wxRED_BRUSH);
		ctx->DrawEllipse(w - 40, 10, 20, 20);

		ctx->SetBrush(*wxGREEN_BRUSH);
		ctx->DrawEllipse(w - 70, 10, 20, 20);

		ctx->SetBrush(*wxYELLOW_BRUSH);
		ctx->DrawEllipse(w - 100, 10, 20, 20);

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
			SetSize(this->GetParent()->GetSize().GetWidth(),this->GetParent()->GetSize().GetHeight() - 30);
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

