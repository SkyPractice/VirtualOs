#include "DeskTop.h"
#include "../CustomMsgs/WindowCreationEvent.h"
#include "../CustomMsgs/WindowTerminationEvent.h"
#include "../CustomMsgs/ButtonCreationEvent.h"
#include "../Lexer/Lexer.h"


wxDECLARE_EVENT(MY_CUSTOM_EVENT, WindowCreationEvent);
wxDEFINE_EVENT(MY_CUSTOM_EVENT, WindowCreationEvent);

wxDECLARE_EVENT(WINDOW_TERMINATION_EVENT, WindowTerminationEvent); // ProcessTerminationEvent
wxDEFINE_EVENT(WINDOW_TERMINATION_EVENT, WindowTerminationEvent);


WindowCreationEvent::WindowCreationEvent(std::string name, Process * process,
	std::vector<Interrupt>* int_vec, std::mutex* kernel_mutex, wxBitmap img, WindowType t) :
	wxCommandEvent(MY_CUSTOM_EVENT, wxID_ANY),
	window_name(name), process_caller(process), type(t), interrupts_vec(int_vec), kernel_mut(kernel_mutex), process_img(img) {};

WindowTerminationEvent::WindowTerminationEvent(int id) : wxCommandEvent(WINDOW_TERMINATION_EVENT, wxID_ANY),
	window_id(id) {};

DeskTop::DeskTop(wxWindow* parent) : wxPanel(parent) {

	int w, h;
	this->GetSize(&w, &h);
	start_menu = new StartMenu(this, 300, 350, 10, h - 10 - 350, 15);
	start_menu->Hide();
	
	task_bar = new TaskBar(this, 30, start_menu);
	task_bar->SetSize(wxSize(w, 30));
	task_bar->SetPosition(wxPoint(0, h - 50));

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(wxTransparentColour);
	wxImage img;
	for(auto dir : fs::directory_iterator("./")){
		if(dir.path().filename().string().starts_with("wallpaper") && !dir.is_directory()){
			img = wxImage(dir.path().filename().string());
		}
	}

	bmp = wxBitmap(img);
	this->Bind(wxEVT_PAINT, &DeskTop::onPaint, this);
	this->Bind(wxEVT_SIZE, [&](wxSizeEvent& evt) -> void {
		task_bar->SetSize(wxSize(evt.GetSize().GetWidth(), 30));
		task_bar->SetPosition(wxPoint(0, evt.GetSize().GetHeight() - 30));
		start_menu->SetSize(wxSize(300, 350));
		start_menu->SetPosition(wxPoint(10, evt.GetSize().GetHeight() - 10 - 350 - 30));
		});

	this->Bind(MY_CUSTOM_EVENT, [&](WindowCreationEvent& evt) {
		ProcessWindow* process_window = new ProcessWindow(this, evt.window_name, evt.interrupts_vec,
			evt.kernel_mut, evt.process_caller, evt.process_img, evt.type);
		
		if(evt.type == GUIWindow){
			evt.process_caller->interpreter.program_scope->variables["window"] = 
				std::make_shared<ControlHandle>(FrameControlType, process_window);
		}
		evt.process_caller->window = process_window;
		process_windows.push_back(process_window);
		evt.process_caller->suspended = false;
		task_bar->Refresh();
		});

	this->Bind(WINDOW_TERMINATION_EVENT, [&](WindowTerminationEvent& evt) {
		auto itr = std::find_if(process_windows.begin(), process_windows.end(), [&](ProcessWindow* window)
			{ return window->id == evt.window_id; });

		if (itr != process_windows.end()) {	
			(*itr)->Destroy();
			delete (*itr)->process; // deleting the process automatically deletes the window
			process_windows.erase(itr);
			task_bar->Refresh();
		}
		});
	this->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent& evt){
		if(start_menu->IsShown())
			start_menu->Show(false);
	});
	this->Bind(wxEVT_SIZE, [&](wxSizeEvent& s) {
		Refresh();
		s.Skip();
		});


}

void DeskTop::addIcon(OsAppIcon* icon) {
	icon->SetPosition(wxPoint(0, icons.size() * icon->GetSize().y + icons.size() * 20));
	icon->Refresh();
	icon->Layout();
	Refresh();
	icons.push_back(icon);
}
void DeskTop::init() {

	//addIcon(new OsAppIcon(this, "test_icon.png", "test_app.txt", "app", interrupt_vec, kernel_mut));
	//addIcon(new OsAppIcon(this, "test_icon.png", "test_app.txt", "app", interrupt_vec, kernel_mut));
	//addIcon(new OsAppIcon(this, "test_icon.png", "test_app.txt", "app", interrupt_vec, kernel_mut));
	for (const auto& ic : icons)
		ic->Refresh();
}

void DeskTop::onPaint(wxPaintEvent& evt) {
	wxAutoBufferedPaintDC dc(this);
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
		ctx->SetBrush(*wxWHITE_BRUSH);
		ctx->DrawRectangle(0, 0, w, h);
		ctx->DrawBitmap(bmp, x, y, w, h);

		delete ctx;
	}
}
