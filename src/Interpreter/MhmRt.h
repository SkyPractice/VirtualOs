#pragma once
#include <memory>
#include <string>
#include <vector>
#include <wx/wx.h>

enum RunTimeValType {
	NumType, StringType, BoolType, SysCallType, ArrayType, HANDLETYPE
};

enum HandleType{
	FileHandleType, ControlHandleType
};

enum ControlType{
	LabelControlType, ButtonControlType, ImageControlType, FrameControlType
};

class RunTimeVal {
public:
	RunTimeValType type;

	RunTimeVal(RunTimeValType t) : type(t) {};

	virtual ~RunTimeVal() {};
};

class NumVal : public RunTimeVal {
public:
	double number;

	NumVal(double num) : RunTimeVal(NumType), number(num) {};
};

class BoolVal : public RunTimeVal {
public:
	bool val;

	BoolVal(bool value) : RunTimeVal(BoolType), val(value) {};
};

class StringVal : public RunTimeVal {
public:
	std::string str;

	StringVal(std::string value) : RunTimeVal(StringType), str(value) {};
};

class SysCallVal : public RunTimeVal {
public:
	std::string name;
	std::vector<std::shared_ptr<RunTimeVal>> args;

	SysCallVal(std::string call_name, std::vector<std::shared_ptr<RunTimeVal>> call_args) : RunTimeVal(SysCallType),
		name(call_name), args(call_args) {};
};

class ArrayVal : public RunTimeVal {
public:
	std::vector<std::shared_ptr<RunTimeVal>> elms;

	ArrayVal(std::vector<std::shared_ptr<RunTimeVal>> elements): RunTimeVal(ArrayType), elms(elements) {};
};

class Handle : public RunTimeVal {
public:
	HandleType type;

	Handle(HandleType t): RunTimeVal(HANDLETYPE), type(t) {};
};

class ControlHandle : public Handle {
public:
	ControlType control_type;
	wxWindow* window;

	ControlHandle(ControlType control_t, wxWindow* wind): Handle(ControlHandleType), control_type(control_t),
		window(wind) {};

};

