#pragma once
#include <memory>
#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/app.h>
#include "../Parser/Ast.h"
#include <unordered_map>

enum RunTimeValType {
	NumType, StringType, BoolType, SysCallType, ArrayType, HANDLETYPE, FunctionType, StructType
};

enum HandleType{
	FileHandleType, ControlHandleType
};

enum ControlType{
	LabelControlType, ButtonControlType, ImageControlType, FrameControlType, InputControlType,
	PanelControlType
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

class FunctionVal : public RunTimeVal {
public:
	std::vector<std::string> args;
	std::vector<std::shared_ptr<StatementObj>> stmts;
	std::vector<std::pair<std::string, std::shared_ptr<RunTimeVal>>> captured_values_by_val;
	
	FunctionVal(std::vector<std::string> args_arg, 
		std::vector<std::shared_ptr<StatementObj>> stmts_arg,
		std::vector<std::pair<std::string, std::shared_ptr<RunTimeVal>>> captured_vals):
		RunTimeVal(FunctionType), args(args_arg), stmts(stmts_arg), captured_values_by_val(captured_vals){};
};

class StructVal : public RunTimeVal {
public:
	std::unordered_map<std::string, std::shared_ptr<RunTimeVal>> values;

	StructVal(std::unordered_map<std::string, std::shared_ptr<RunTimeVal>> vals): 
		RunTimeVal(StructType), values(vals) {};
};