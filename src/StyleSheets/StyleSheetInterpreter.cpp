#define _CRT_SECURE_NO_WARNINGS
#include "../Interpreter/Interpreter.h"
#include "../Processes/CustomPanel.h"
#include "../Processes/ProcessWindow.h"
#include "../Processes/CustomImage.h"

class StyleSheetInitializer {
public:
    using FuncRet = std::shared_ptr<RunTimeVal>;
    using FuncArgs = std::vector<std::shared_ptr<RunTimeVal>>;
    static FuncRet setWindowBkColor(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<NumVal> r = std::dynamic_pointer_cast<NumVal>(args[1]);
        shared_ptr<NumVal> g = std::dynamic_pointer_cast<NumVal>(args[2]);
        shared_ptr<NumVal> b = std::dynamic_pointer_cast<NumVal>(args[3]);
        shared_ptr<NumVal> a = std::dynamic_pointer_cast<NumVal>(args[4]);

        if(handle->control_type == PanelControlType){
            CustomPanel* panel = dynamic_cast<CustomPanel*>(handle->window);
            panel->bk_clr = wxColour(r->number, g->number, b->number, a->number);
            panel->Refresh();
        }
        else if(handle->control_type == FrameControlType){
            ProcessWindow* window = dynamic_cast<ProcessWindow*>(handle->window);
            window->bk_clr = wxColour(r->number, g->number, b->number, a->number);
            window->Refresh();
        }
        return nullptr;
    }
    static FuncRet setWindowTxtColor(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<NumVal> r = std::dynamic_pointer_cast<NumVal>(args[1]);
        shared_ptr<NumVal> g = std::dynamic_pointer_cast<NumVal>(args[2]);
        shared_ptr<NumVal> b = std::dynamic_pointer_cast<NumVal>(args[3]);
        shared_ptr<NumVal> a = std::dynamic_pointer_cast<NumVal>(args[4]);

        handle->window->SetForegroundColour(wxColour(r->number, g->number, b->number, a->number));
        handle->window->Refresh();
        
        return nullptr;
    }
    static FuncRet setWindowBorderColor(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<NumVal> r = std::dynamic_pointer_cast<NumVal>(args[1]);
        shared_ptr<NumVal> g = std::dynamic_pointer_cast<NumVal>(args[2]);
        shared_ptr<NumVal> b = std::dynamic_pointer_cast<NumVal>(args[3]);
        shared_ptr<NumVal> a = std::dynamic_pointer_cast<NumVal>(args[4]);

        if(handle->control_type == PanelControlType){
            CustomPanel* panel = dynamic_cast<CustomPanel*>(handle->window);
            panel->border_clr = wxColour(r->number, g->number, b->number, a->number);
            panel->Refresh();
        }

        return nullptr;
    }
    static FuncRet setWindowBorderWeight(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<NumVal> w = std::dynamic_pointer_cast<NumVal>(args[1]);
        if(handle->control_type == PanelControlType){
            CustomPanel* panel = dynamic_cast<CustomPanel*>(handle->window);
            panel->border_weight = w->number;
            panel->Refresh();
        }

        return nullptr;
    }
    static FuncRet setWindowBorderRadius(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<NumVal> w = std::dynamic_pointer_cast<NumVal>(args[1]);
        if(handle->control_type == PanelControlType){
            CustomPanel* panel = dynamic_cast<CustomPanel*>(handle->window);
            panel->border_radius = w->number;
            panel->Refresh();
        }
        else if(handle->control_type == ImageControlType){
            CustomImage* panel = dynamic_cast<CustomImage*>(handle->window);
            panel->border_rad = w->number;
            panel->Refresh();
        }
        return nullptr;
    }
    static FuncRet setWindowPosition(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<NumVal> x = std::dynamic_pointer_cast<NumVal>(args[1]);
        shared_ptr<NumVal> y = std::dynamic_pointer_cast<NumVal>(args[2]);
        
        wxWindow* window = handle->window;
        window->SetPosition(wxPoint(x->number, y->number));
        window->Refresh();
        

        return nullptr;
    }
    static FuncRet setWindowSize(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<NumVal> x = std::dynamic_pointer_cast<NumVal>(args[1]);
        shared_ptr<NumVal> y = std::dynamic_pointer_cast<NumVal>(args[2]);
        wxWindow* window = handle->window;
        window->SetSize(wxSize(x->number, y->number));
        window->Refresh();
        

        return nullptr;
    }
    static FuncRet setWindowFont(FuncArgs args, Interpreter* interp){
        shared_ptr<Handle> not_casted_handle = std::dynamic_pointer_cast<Handle>(args[0]);
        shared_ptr<ControlHandle> handle = std::dynamic_pointer_cast<ControlHandle>(not_casted_handle);
        shared_ptr<StringVal> font_family = std::dynamic_pointer_cast<StringVal>(args[1]);
        shared_ptr<NumVal> font_size = std::dynamic_pointer_cast<NumVal>(args[2]);
        shared_ptr<NumVal> font_weight = std::dynamic_pointer_cast<NumVal>(args[3]);
        wxFontInfo fnt_info(font_size->number);
        fnt_info.FaceName(font_family->str);
        fnt_info.Weight(font_weight->number);
        fnt_info.AntiAliased(true);
        handle->window->SetFont(
            wxFont(fnt_info)
        );

        return nullptr;
    }
    StyleSheetInitializer(){

        SystemCalls::native_functions.insert({"set_window_bk_color", StyleSheetInitializer::setWindowBkColor});
        SystemCalls::native_functions.insert({"set_window_bk_colour", StyleSheetInitializer::setWindowBkColor});
        SystemCalls::native_functions.insert({"set_window_border_color", StyleSheetInitializer::setWindowBorderColor});
        SystemCalls::native_functions.insert({"set_window_border_colour", StyleSheetInitializer::setWindowBorderColor});
        SystemCalls::native_functions.insert({"set_window_border_weight", StyleSheetInitializer::setWindowBorderWeight});
        SystemCalls::native_functions.insert({"set_window_border_radius", StyleSheetInitializer::setWindowBorderRadius});
        SystemCalls::native_functions.insert({"set_window_pos", StyleSheetInitializer::setWindowPosition});
        SystemCalls::native_functions.insert({"set_window_size", StyleSheetInitializer::setWindowSize});
        SystemCalls::native_functions.insert({"set_window_font", StyleSheetInitializer::setWindowFont});
        SystemCalls::native_functions.insert({"set_window_txt_color", StyleSheetInitializer::setWindowTxtColor});
        SystemCalls::native_functions.insert({"set_window_txt_colour", StyleSheetInitializer::setWindowTxtColor});

    }
};

static StyleSheetInitializer initializer;