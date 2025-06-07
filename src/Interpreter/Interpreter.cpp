#define _CRT_SECURE_NO_WARNINGS
#include "Interpreter.h"
#include "../Processes/Process.h"
#include <algorithm>
#include <condition_variable>
#include "../CustomMsgs/ButtonCreationEvent.h"
#include "../Processes/BkTransparentLabel.h"

std::unordered_map<std::string, std::function<shared_ptr<RunTimeVal>(std::vector<shared_ptr<RunTimeVal>>, 
	Process*)>> SystemCalls::sys_calls =
{ { "print", [&](std::vector<shared_ptr<RunTimeVal>> args, Process* caller) -> shared_ptr<RunTimeVal> {
	if (args[0]->type == StringType) {
		shared_ptr<StringVal> str = std::dynamic_pointer_cast<StringVal>(args[0]);
		if(caller->output_label)
			caller->output_label->SetSmoothLabelText(caller->output_label->GetLabelText() + "\n" + str->str);
	}
	else if (args[0]->type == NumType) {
		shared_ptr<NumVal> str = std::dynamic_pointer_cast<NumVal>(args[0]);
		if(caller->output_label)
			caller->output_label->SetSmoothLabelText(caller->output_label->GetLabelText() + "\n" + std::to_string(str->number));
	}
	else if (args[0]->type == BoolType) {
		shared_ptr<BoolVal> str = std::dynamic_pointer_cast<BoolVal>(args[0]);
		if (caller->output_label)
		{
			if (str->val)
				caller->output_label->SetSmoothLabelText(caller->output_label->GetLabelText() + "\n" + "true");
			else
				caller->output_label->SetSmoothLabelText(caller->output_label->GetLabelText() + "\n" + "false");
		}
	}
	return nullptr;

	}
	}, 
	{ "sleep", [&](std::vector<shared_ptr<RunTimeVal>> args, Process* caller) -> shared_ptr<RunTimeVal> {
		if (args[0]->type == NumType) {
			std::shared_ptr<NumVal> val = std::dynamic_pointer_cast<NumVal>(args[0]);
			std::lock_guard<std::mutex> locker(caller->process_mutex);
			caller->sleep_duration = val->number;
			caller->sleep_time = std::chrono::high_resolution_clock::now();
			caller->isSleeping = true;
			caller->suspended = true;
		}

		return nullptr;
	}},
	{ "interrupt", [&](std::vector<shared_ptr<RunTimeVal>> args, Process* caller) -> shared_ptr<RunTimeVal> {
		return nullptr;
	}},
	{ "directory_iterator", [&](std::vector<shared_ptr<RunTimeVal>> args, Process* caller) -> shared_ptr<RunTimeVal> {
		std::shared_ptr<ArrayVal> val = std::make_shared<ArrayVal>(std::vector<std::shared_ptr<RunTimeVal>>());
		for(auto& p : fs::directory_iterator(std::dynamic_pointer_cast<StringVal>(args[0])->str)){
			val->elms.push_back(std::make_shared<StringVal>(p.path().string()));
		};
		caller->interpreter.current_scope->variables["eax"] = val;

		return nullptr;

	}},
	{ "recursive_directory_iterator", [&](std::vector<shared_ptr<RunTimeVal>> args, Process* caller) -> shared_ptr<RunTimeVal> {
		std::shared_ptr<ArrayVal> val = std::make_shared<ArrayVal>(std::vector<std::shared_ptr<RunTimeVal>>());
		for(auto& p : fs::recursive_directory_iterator(std::dynamic_pointer_cast<StringVal>(args[0])->str)){
			val->elms.push_back(std::make_shared<StringVal>(p.path().string()));
		};
		caller->interpreter.current_scope->variables["eax"] = val;

		return nullptr;

	}}
	
};

std::unordered_map<std::string, std::function<shared_ptr<RunTimeVal>(std::vector<shared_ptr<RunTimeVal>>,
	Interpreter*)>>
		SystemCalls::native_functions = {
			{
				"array_size", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
					std::shared_ptr<ArrayVal> array = std::dynamic_pointer_cast<ArrayVal>(args[0]);

					return std::make_shared<NumVal>(static_cast<double>(array->elms.size()));
				}
			},
			{
				"create_button", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
					std::shared_ptr<Handle> not_fully_casted_window_handle = 
						std::dynamic_pointer_cast<Handle>(args[0]);
					
					std::shared_ptr<ControlHandle> window_handle = 
						std::dynamic_pointer_cast<ControlHandle>(not_fully_casted_window_handle);
					
					std::shared_ptr<StringVal> button_str = std::dynamic_pointer_cast<StringVal>(args[1]);

					std::shared_ptr<NumVal> button_x = std::dynamic_pointer_cast<NumVal>(args[2]);
					std::shared_ptr<NumVal> button_y = std::dynamic_pointer_cast<NumVal>(args[3]);
					std::shared_ptr<NumVal> button_width = std::dynamic_pointer_cast<NumVal>(args[4]);
					std::shared_ptr<NumVal> button_height = std::dynamic_pointer_cast<NumVal>(args[5]);
					std::shared_ptr<NumVal> font_size = std::dynamic_pointer_cast<NumVal>(args[6]);

					wxButton* btn = nullptr;

					std::function<void()> fun = [&, window_handle, button_str, button_x, button_y, 
						button_width, button_height](){
						btn = new wxButton(
							window_handle->window,
							wxID_ANY,
							button_str->str,
							wxPoint(button_x->number, button_y->number + 40),
							wxSize(button_width->number, button_height->number)
						);
						btn->SetFont(wxFont(font_size->number, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
							wxFONTWEIGHT_NORMAL));
					};

					if(!wxThread::IsMain()){
						std::condition_variable v;
						std::mutex mut;
						std::unique_lock<std::mutex> lock(mut);


						window_handle->window->CallAfter([&, window_handle, button_str,
							button_x, button_y, button_width, button_height, font_size](){
							{
								std::unique_lock<std::mutex> locker(mut);
								fun();
							}
							v.notify_all();
						});

						v.wait(lock, [&]()->bool{ return btn; });

					} else fun();
					

					return std::make_shared<ControlHandle>(ButtonControlType, 
						btn);

				}
			},
				{
				"create_label", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
					std::shared_ptr<Handle> not_fully_casted_window_handle = 
						std::dynamic_pointer_cast<Handle>(args[0]);
					
					std::shared_ptr<ControlHandle> window_handle = 
						std::dynamic_pointer_cast<ControlHandle>(not_fully_casted_window_handle);

					std::shared_ptr<StringVal> str = std::dynamic_pointer_cast<StringVal>(args[1]);
					std::shared_ptr<NumVal> x = std::dynamic_pointer_cast<NumVal>(args[2]);
					std::shared_ptr<NumVal> y = std::dynamic_pointer_cast<NumVal>(args[3]);
					std::shared_ptr<NumVal> w = std::dynamic_pointer_cast<NumVal>(args[4]);
					std::shared_ptr<NumVal> h = std::dynamic_pointer_cast<NumVal>(args[5]);
					std::shared_ptr<NumVal> font_size = std::dynamic_pointer_cast<NumVal>(args[6]);

					BkTransparentLabel* label;

					std::function<void()> fun = [&, window_handle, str,
						x, y, w, h, font_size](){
						label = new BkTransparentLabel(
							window_handle->window,
							x->number, y->number + 40,
							w->number, h->number,
							str->str,
							font_size->number
						);
						label->SetFont(wxFont(font_size->number, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
							wxFONTWEIGHT_NORMAL));
						};

					if(!wxThread::IsMain()){

						std::condition_variable v;
						std::mutex mut;
						std::unique_lock<std::mutex> lock(mut);
						window_handle->window->CallAfter([&, window_handle, str,
							x, y, w, h, font_size](){

							{
								std::unique_lock<std::mutex> locker(mut);
								fun();
							}
							v.notify_all();
						});

						v.wait(lock, [&]()->bool{return label;});

					} else fun();

					return std::make_shared<ControlHandle>(LabelControlType, 
						label);
					
				}
			},
			{
				"create_input", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
					std::shared_ptr<Handle> not_fully_casted_window_handle = 
						std::dynamic_pointer_cast<Handle>(args[0]);
					
					std::shared_ptr<ControlHandle> window_handle = 
						std::dynamic_pointer_cast<ControlHandle>(not_fully_casted_window_handle);
					
					std::shared_ptr<StringVal> input_str = std::dynamic_pointer_cast<StringVal>(args[1]);
					
					std::shared_ptr<NumVal> x = std::dynamic_pointer_cast<NumVal>(args[2]);
					std::shared_ptr<NumVal> y = std::dynamic_pointer_cast<NumVal>(args[3]);
					std::shared_ptr<NumVal> width = std::dynamic_pointer_cast<NumVal>(args[4]);
					std::shared_ptr<NumVal> height = std::dynamic_pointer_cast<NumVal>(args[5]);
					std::shared_ptr<NumVal> font_size = std::dynamic_pointer_cast<NumVal>(args[6]);

					wxTextCtrl* txt_input = nullptr;

					std::function<void()> fun = [&, window_handle, input_str,
						x, y, width, height, font_size](){
						txt_input = new wxTextCtrl(
							window_handle->window,
							wxID_ANY,
							input_str->str,
							wxPoint(x->number, y->number + 40),
							wxSize(width->number, height->number)
						);
						txt_input->SetFont(wxFont(font_size->number, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
							wxFONTWEIGHT_NORMAL));
					};

					std::condition_variable v;
					std::mutex mut;
					std::unique_lock<std::mutex> lock(mut);

					if(!wxThread::IsMain()){
					window_handle->window->CallAfter([&, window_handle, input_str,
						x, y, width, height, font_size](){
							{
								std::unique_lock<std::mutex> locker(mut);
								fun();
							}
							v.notify_all();
						});

						v.wait(lock, [&]() -> bool { return txt_input; });
					} else fun();
					return std::make_shared<ControlHandle>(ButtonControlType, 
						txt_input);

				}
			},
			{
				"get_window_size", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
					std::shared_ptr<Handle> not_fully_casted_window_handle = 
						std::dynamic_pointer_cast<Handle>(args[0]);
					
					std::shared_ptr<ControlHandle> window_handle = 
						std::dynamic_pointer_cast<ControlHandle>(not_fully_casted_window_handle);

					// the first element in the array is the width, the second is the height :skull:

					std::vector<std::shared_ptr<RunTimeVal>> arr;
					arr.push_back(std::make_shared<NumVal>(window_handle->window->GetSize().GetWidth()));
					arr.push_back(std::make_shared<NumVal>(window_handle->window->GetSize().GetHeight()));

					std::shared_ptr<ArrayVal> val = std::make_shared<ArrayVal>(arr);

					return val;

				}
			},
			{ "array_push_back", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
				std::shared_ptr<ArrayVal> arr = std::dynamic_pointer_cast<ArrayVal>(args[0]);
				arr->elms.push_back(args[1]);

				return arr;

			} },
			{ "free_handle", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
				std::shared_ptr<Handle> not_fully_casted_window_handle = 
					std::dynamic_pointer_cast<Handle>(args[0]);
				
				if(not_fully_casted_window_handle->type == ControlHandleType){
					std::shared_ptr<ControlHandle> window_handle = 
						std::dynamic_pointer_cast<ControlHandle>(not_fully_casted_window_handle);
					window_handle->window->GetParent()->CallAfter([&, window_handle](){
						window_handle->window->GetParent()->RemoveChild(
							window_handle->window
						);
					});

					window_handle->window->Destroy();
				}

				return nullptr;

			} },
			{ "bind_event_to_handle", [&](std::vector<shared_ptr<RunTimeVal>> args, Interpreter* interp) -> shared_ptr<RunTimeVal> {
				std::shared_ptr<Handle> not_fully_casted_window_handle = 
					std::dynamic_pointer_cast<Handle>(args[0]);
				try{
				if(not_fully_casted_window_handle->type == ControlHandleType){
					std::shared_ptr<ControlHandle> window_handle = 
						std::dynamic_pointer_cast<ControlHandle>(not_fully_casted_window_handle);

					std::shared_ptr<StringVal> evt_type = std::dynamic_pointer_cast<StringVal>(args[1]);
					std::shared_ptr<FunctionVal> func_val = std::dynamic_pointer_cast<FunctionVal>(args[2]);

					if(evt_type->str == "click_event"){
						window_handle->window->CallAfter(
							[&, window_handle, func_val, interp](){
								window_handle->window->Bind(wxEVT_LEFT_DOWN, [&, interp, func_val, window_handle](wxMouseEvent& evt){
								auto scope = std::make_shared<Scope>(interp->program_scope);
								interp->current_scope = scope;
								for(auto& stmt : func_val->stmts){
									interp->evaluate(stmt);
								}
								interp->current_scope = interp->current_scope->parent_scope;
								});					
							}
						);
					}

					return std::make_shared<BoolVal>(true);
				
				}} catch(std::exception& exp){
					wxLogError(exp.what());
				}


				return std::make_shared<BoolVal>(false);
			} }
		};

std::shared_ptr<RunTimeVal> Interpreter::nextStatement() {
	if (program_scope->c_stmt != stmts.end()) { 
		std::shared_ptr<RunTimeVal> res = evaluate(*current_scope->c_stmt);

		if (!switched) {
			current_scope->c_stmt++;
			if (current_scope->c_stmt == current_scope->c_stmt_end) {
				if (current_scope->parent_scope != nullptr) {
					current_scope = current_scope->parent_scope;
					current_scope->c_stmt++;
				}
				else return make_shared<BoolVal>(false);
			}
		}
		else { switched = false; }
		if (!res) return make_shared<BoolVal>(true);

		if (res->type != SysCallType)
			return make_shared<BoolVal>(true);
		else return res;

	}
	return make_shared<BoolVal>(false);
}

std::shared_ptr<RunTimeVal> Interpreter::evaluate(std::shared_ptr<StatementObj> statement) {
	switch (statement->type)
	{
	case BinaryExpressionType:
		return evaluateBinaryExpr(std::dynamic_pointer_cast<BinaryExpressionObj>(statement));
	case NumExprType:
		return make_shared<NumVal>(std::dynamic_pointer_cast<NumExpression>(statement)->num);
	case StringExprType:
		return make_shared<StringVal>(std::dynamic_pointer_cast<StringExpression>(statement)->str);
	case FuncCallExprType:
		return evaluateFunctionCall(std::dynamic_pointer_cast<FunctionCall>(statement));
	case VariableDeclExprType:
		return evaluateVariableDecl(std::dynamic_pointer_cast<VariableDecleration>(statement));
	case IdentifierExprType:
		return evaluateIdentifier(std::dynamic_pointer_cast<IdentifierExpr>(statement));
	case BooleanExprType:
		return make_shared<BoolVal>(std::dynamic_pointer_cast<BooleanExpression>(statement)->val);
	case IfStatementType:
		return evaluateIfStatement(std::dynamic_pointer_cast<IfStatement>(statement));
	case ConditionJmpType:
		return evaluateConditionJmp(std::dynamic_pointer_cast<ConditionJmpStatement>(statement));
	case WhileLoopStatementType:
		return evaluateWhileLoop(std::dynamic_pointer_cast<WhileLoopStatement>(statement));
	case VariableReInitStatementType:
		return evaluateVariableReInit(std::dynamic_pointer_cast<VariableReInitStatement>(statement));
	case ForLoopStatementType:
		return evaluateForLoop(std::dynamic_pointer_cast<ForLoopStatement>(statement));
	case ConditionActionJmpType:
		return evaluateConditionActionJmp(std::dynamic_pointer_cast<ConditionActionJmpStatement>(statement));
	case ArrayExpressionType:
		return evaluateArrayExpr(std::dynamic_pointer_cast<ArrayExpr>(statement));
	case IndexAccessExpressionType:
		return evaluateIndexAccessExpr(std::dynamic_pointer_cast<IndexAccessExpr>(statement));
	case BreakStatementType:
		return evaluateBreakStatement();
	case ContinueStatementType:
		return evaluateContinueStatement();
	case LambdaExprType:
		return evaluateLambdaExpr(std::dynamic_pointer_cast<LambdaExpression>(statement));
	default:
		break;
	}
	return nullptr;
};
std::shared_ptr<RunTimeVal> Interpreter::evaluateBinaryExpr(std::shared_ptr<BinaryExpressionObj> expr) { 
	const auto left = evaluate(expr->left);
	const auto right = evaluate(expr->right);

	if (left->type == NumType && right->type == NumType) {
		const auto real_left = std::dynamic_pointer_cast<NumVal>(left);
		const auto real_right = std::dynamic_pointer_cast<NumVal>(right);

		return evaluateNumericBinaryExpr(real_left, real_right, expr->op);
	}
	else if (left->type == BoolType && right->type == BoolType) {
		const auto real_left = std::dynamic_pointer_cast<BoolVal>(left);
		const auto real_right = std::dynamic_pointer_cast<BoolVal>(right);

		return evaluateBooleanBinaryExpr(real_left, real_right, expr->op);
	}

	throw std::exception("Unexcepted Expression");

};
std::shared_ptr<RunTimeVal> Interpreter::evaluateNumericBinaryExpr(
	std::shared_ptr<NumVal> left, std::shared_ptr<NumVal> right, std::string op
) { 

	double result = 0;

	const double left_val = left->number;
	const double right_val = right->number;


	if (op == "+")
		result = left_val + right_val;
	else if (op == "-")
		result = left_val - right_val;
	else if (op == "*")
		result = left_val * right_val;
	else if (op == "/")
		result = left_val / right_val;
	else if (op == "%")
		result = static_cast<int>(left_val) % static_cast<int>(right_val);
	else if (op == ">")
		return make_shared<BoolVal>(left_val > right_val);
	else if (op == "<")
		return make_shared<BoolVal>(left_val < right_val);
	else if (op == "=!")
		return make_shared<BoolVal>(left_val != right_val);
	else if (op == "==")
		return make_shared<BoolVal>(left_val == right_val);
	else if (op == "=>")
		return make_shared<BoolVal>(left_val >= right_val);
	else if (op == "=<")
		return make_shared<BoolVal>(left_val <= right_val);

	return std::make_shared<NumVal>(result);
};

std::shared_ptr<RunTimeVal> Interpreter::evaluateBooleanBinaryExpr(std::shared_ptr<BoolVal> left,
	std::shared_ptr<BoolVal> right, std::string op) {
	const bool left_val = left->val;
	const bool right_val = right->val;

	if (op == "==")
		return make_shared<BoolVal>(left_val == right_val);
	else if (op == "=!")
		return make_shared<BoolVal>(left_val != right_val);

	return make_shared<BoolVal>(false);
}

std::shared_ptr<RunTimeVal> Interpreter::evaluateFunctionCall(std::shared_ptr<FunctionCall> call) {
	if (SystemCalls::sys_calls.find(call->func_name->identifer_name) != SystemCalls::sys_calls.end()) {
		std::vector<shared_ptr<RunTimeVal>> values;

		for (const auto& arg : call->args) {
			values.push_back(evaluate(arg));
		}

		return std::make_shared<SysCallVal>(call->func_name->identifer_name, values);
	} else if (SystemCalls::native_functions.find(call->func_name->identifer_name)
		!= SystemCalls::native_functions.end()){
				std::vector<shared_ptr<RunTimeVal>> values;

		for (const auto& arg : call->args) {
			values.push_back(evaluate(arg));
		}
		return SystemCalls::native_functions[call->func_name->identifer_name](values, this);
	}
	throw std::exception("No Function Calls Implemented Except Sys Calls");
}

std::shared_ptr<RunTimeVal> Interpreter::evaluateVariableDecl(std::shared_ptr<VariableDecleration> decl) {
	
	const auto val = evaluate(decl->val);
	current_scope->addVar(decl->name, val);

	return nullptr;
}

std::shared_ptr<RunTimeVal> Interpreter::evaluateIdentifier(std::shared_ptr<IdentifierExpr> iden) {
	const auto val = current_scope->lookUpVar(iden->identifer_name);
	if (val)
		return val;
	return nullptr;
}

std::shared_ptr<RunTimeVal> Interpreter::evaluateIfStatement(std::shared_ptr<IfStatement> statement) {
	const auto expr_val = evaluate(statement->expr);
	if (expr_val->type != BoolType)
		throw std::exception("boolean expression excepted");
	const auto real_expr_val = std::dynamic_pointer_cast<BoolVal>(expr_val);

	if (real_expr_val->val) {
		std::shared_ptr<Scope> if_scope = make_shared<Scope>(current_scope);
		current_scope = if_scope;
		current_scope->c_stmt = statement->stmts.begin();
		current_scope->c_stmt_end = statement->stmts.end();
		switched = true;
		return nullptr;
	}

	if (!statement->else_if_stmts.empty()) {
		for (auto& else_if_stmt : statement->else_if_stmts) {
			const auto real_expression_else_if_val = std::dynamic_pointer_cast<BoolVal>(evaluate(else_if_stmt->expr));

			if (real_expression_else_if_val->val) {
				std::shared_ptr<Scope> else_if_scope = make_shared<Scope>(current_scope);
				current_scope = else_if_scope;
				current_scope->c_stmt = else_if_stmt->stmts.begin();
				current_scope->c_stmt_end = else_if_stmt->stmts.end();
				switched = true;
				return nullptr;
			}
		}
	}
	
	if (!statement->else_stmt) return nullptr;

	std::shared_ptr<Scope> else_scope = make_shared<Scope>(current_scope);
	current_scope = else_scope;
	current_scope->c_stmt = statement->else_stmt->stmts.begin();
	current_scope->c_stmt_end = statement->else_stmt->stmts.end();
	switched = true;
	
	return nullptr;
};
std::shared_ptr<RunTimeVal> Interpreter::evaluateVariableReInit(std::shared_ptr<VariableReInitStatement> statement) {
	current_scope->setVar(statement->name, evaluate(statement->val));

	return nullptr;
};
std::shared_ptr<RunTimeVal> Interpreter::evaluateWhileLoop(std::shared_ptr<WhileLoopStatement> statement) {
	const auto expr_val = evaluate(statement->expr);
	if (expr_val->type != BoolType)
		throw std::exception("boolean expression excepted");
	const auto real_expr_val = std::dynamic_pointer_cast<BoolVal>(expr_val);

	if (real_expr_val->val) {
		std::shared_ptr<Scope> while_scope = make_shared<Scope>(current_scope);
		current_scope = while_scope;
		current_scope->c_stmt = statement->stmts.begin();
		current_scope->c_stmt_end = statement->stmts.end();
		switched = true;
		return nullptr;
	}

	return nullptr;
};
std::shared_ptr<RunTimeVal> Interpreter::evaluateForLoop(std::shared_ptr<ForLoopStatement> statement) {
	const auto expr_val = evaluate(statement->expr);
	if (expr_val->type != BoolType)
		throw std::exception("boolean expression excepted");
	const auto real_expr_val = std::dynamic_pointer_cast<BoolVal>(expr_val);

	if (real_expr_val->val) {
		std::shared_ptr<Scope> for_scope = make_shared<Scope>(current_scope);
		current_scope = for_scope;
		current_scope->c_stmt = statement->stmts.begin();
		current_scope->c_stmt_end = statement->stmts.end();
		current_scope->addVar(statement->var_decl->name, evaluate(statement->var_decl->val));
		current_scope->no_del_vars.insert(statement->var_decl->name);
		switched = true;
		return nullptr;
	}

	return nullptr;
};
std::shared_ptr<RunTimeVal> Interpreter::evaluateTryStatement(std::shared_ptr<TryStatement> statement) {return nullptr;};
std::shared_ptr<RunTimeVal> Interpreter::evaluateFuncDecl(std::shared_ptr<FunctionDecleration> statement) { return nullptr;};
std::shared_ptr<RunTimeVal> Interpreter::evaluateThrow(std::shared_ptr<ThrowStatement> statement) { return nullptr;};

std::shared_ptr<RunTimeVal> Interpreter::evaluateBreakStatement() {
	shared_ptr<Scope> sc = current_scope;
	while (sc->parent_scope != nullptr) {
		if ((*(sc->c_stmt_end - 1))->type == ConditionActionJmpType) {
			sc = sc->parent_scope;
			sc->c_stmt++;
			switched = true;
			return nullptr;
		}
		else if ((*(sc->c_stmt_end - 1))->type == ConditionJmpType) {
			sc = sc->parent_scope;
			sc->c_stmt++;
			switched = true;
			return nullptr;
		}
		else {
			sc = sc->parent_scope;
		}
	}
	throw std::exception("No Loop Found To Break Out Off");
}
std::shared_ptr<RunTimeVal> Interpreter::evaluateContinueStatement() {
	shared_ptr<Scope> sc = current_scope;
	while (sc->parent_scope != nullptr) {
		if ((*(sc->c_stmt_end - 1))->type == ConditionActionJmpType) {
			std::shared_ptr<ConditionActionJmpStatement> stmttt = std::dynamic_pointer_cast<ConditionActionJmpStatement>(
				(*(sc->c_stmt_end - 1)));
			sc->c_stmt = stmttt->jmp_stmt;
			switched = true;
			return nullptr;
		}
		else if ((*(sc->c_stmt_end - 1))->type == ConditionJmpType) {
			sc->c_stmt = sc->c_stmt_end - 1;
			switched = true;
			return nullptr;
		}
		else {
			sc = sc->parent_scope;
		}
	}
	throw std::exception("No Loop Found To Continue");
}
std::shared_ptr<RunTimeVal> Interpreter::evaluateConditionJmp(std::shared_ptr<ConditionJmpStatement> statement) {
	const auto expr_val = evaluate(statement->expr);
	if (expr_val->type != BoolType)
		throw std::exception("boolean expression excepted");
	const auto real_expr_val = std::dynamic_pointer_cast<BoolVal>(expr_val);
	if (real_expr_val->val) {
		current_scope->c_stmt = statement->jmp_stmt;
		switched = true;
	}
	current_scope->variables.clear();
	return nullptr;
};

std::shared_ptr<RunTimeVal> Interpreter::evaluateConditionActionJmp(
	std::shared_ptr<ConditionActionJmpStatement> statement) {
	const auto expr_val = evaluate(statement->expr);
	if (expr_val->type != BoolType)
		throw std::exception("boolean expression excepted");
	const auto real_expr_val = std::dynamic_pointer_cast<BoolVal>(expr_val);
	if (real_expr_val->val) {
		current_scope->c_stmt = statement->jmp_stmt;
		evaluate(statement->action);
		switched = true;
	}
	
	std::erase_if(current_scope->variables, [&](const auto& elm) -> bool {
		return current_scope->no_del_vars.find(elm.first) == current_scope->no_del_vars.end(); });
	return nullptr;



}

std::shared_ptr<RunTimeVal> Interpreter::evaluateArrayExpr(std::shared_ptr<ArrayExpr> expr){
	std::vector<std::shared_ptr<RunTimeVal>> arr;
	for(const auto& e : expr->elms){
		arr.push_back(evaluate(e));
	}

	return make_shared<ArrayVal>(arr);
}

std::shared_ptr<RunTimeVal> Interpreter::evaluateIndexAccessExpr(std::shared_ptr<IndexAccessExpr> expr){
	std::shared_ptr<RunTimeVal> current_val = evaluate(make_shared<IdentifierExpr>(expr->var_name));

	for(auto& num : expr->index_path){
		current_val = std::dynamic_pointer_cast<ArrayVal>(current_val)->elms[
			static_cast<int>(std::dynamic_pointer_cast<NumVal>(evaluate(num))->number)];
	}

	return current_val;
};

std::shared_ptr<RunTimeVal> Interpreter::evaluateLambdaExpr(std::shared_ptr<LambdaExpression> expr){
	return std::make_shared<FunctionVal>(expr->args, expr->stmts);
}