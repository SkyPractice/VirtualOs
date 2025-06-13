#pragma once
#include <memory>
#include <string>
#include <vector>

using std::shared_ptr, std::vector, std::make_shared;

enum StatementType {
	ProgramType,
	ExpressionType, BinaryExpressionType, NumExprType, StringExprType,
	BooleanExprType, CharExprType,
	IdentifierExprType, FuncCallExprType, VariableDeclExprType,
	IfStatementType, ElseIfStatementType, ElseStatementType,
	WhileLoopStatementType, ForLoopStatementType,
	VariableReInitStatementType, FunctionDeclerationType,
	BreakStatementType, ContinueStatementType, ThrowStatementType,
	ReturnStatementType, CatchStatementType, TryStatementType,
	ConditionJmpType,
	ConditionActionJmpType,
	ArrayExpressionType,
	IndexAccessExpressionType,
	LambdaExprType,
	IndexReinitType,
	StructDeclerationType
};

enum ErrorType {
	RunTimeException, SyntaxError
};



struct Arg {
	std::string name;
	StatementType type; // for hints only for the future text editor
};



class StatementObj {
public:
	StatementType type;
	std::vector<shared_ptr<StatementObj>>::iterator p_itr;
	std::vector<shared_ptr<StatementObj>>::iterator p_end_itr;
	shared_ptr<StatementObj> p;
	
	StatementObj(StatementType t) : type(t) {};

	virtual ~StatementObj() {};
};

class ProgramObj : public StatementObj {
public:
	vector<shared_ptr<StatementObj>> stmts;
	
	ProgramObj(vector<shared_ptr<StatementObj>> statements) : StatementObj(ProgramType), stmts(statements) {};
};

class ExpressionObj : public StatementObj {
public:
	ExpressionObj(StatementType expr_t) : StatementObj(expr_t) {};
};

struct Error {
	shared_ptr<ExpressionObj> val;
	ErrorType type;
};


class BinaryExpressionObj : public ExpressionObj {
public:
	shared_ptr<ExpressionObj> left;
	shared_ptr<ExpressionObj> right;
	std::string op; // operator

	BinaryExpressionObj(shared_ptr<ExpressionObj> left_arg, shared_ptr<ExpressionObj> right_arg,
		std::string op_arg) : ExpressionObj(BinaryExpressionType), left(left_arg), right(right_arg), op(op_arg) {};
};

class NumExpression : public ExpressionObj {
public:
	double num;

	NumExpression(double num_arg) : ExpressionObj(NumExprType), num(num_arg) {};
};

class StringExpression : public ExpressionObj {
public:
	std::string str;

	StringExpression(std::string Str) : ExpressionObj(StringExprType), str(Str) {};
};

class BooleanExpression : public ExpressionObj {
public:
	bool val;

	BooleanExpression(bool value) : ExpressionObj(BooleanExprType), val(value) {};
};

class CharExpression : public ExpressionObj {
public:
	char ch;

	CharExpression(char character) : ExpressionObj(CharExprType), ch(character) {};
};

class IdentifierExpr : public ExpressionObj {
public:
	std::string identifer_name;

	IdentifierExpr(std::string name) : ExpressionObj(IdentifierExprType), identifer_name(name) {};
};

class VariableDecleration : public StatementObj {
public:
	std::string name;
	shared_ptr<ExpressionObj> val;

	VariableDecleration(std::string var_name, shared_ptr<ExpressionObj> value) : StatementObj(VariableDeclExprType),
		name(var_name), val(value) {};
};

class FunctionCall : public ExpressionObj {
public:
	shared_ptr<IdentifierExpr> func_name;
	std::vector<shared_ptr<ExpressionObj>> args;

	FunctionCall(shared_ptr<IdentifierExpr> func_name_arg, std::vector<shared_ptr<ExpressionObj>> args_arg) :
		ExpressionObj(FuncCallExprType), func_name(func_name_arg), args(args_arg) {};
};

class ElseStatement : public StatementObj {
public:
	std::vector<shared_ptr<StatementObj>> stmts;

	ElseStatement(std::vector<shared_ptr<StatementObj>> stmts_arg) :
		StatementObj(ElseStatementType), stmts(stmts_arg) {};
};

class ElseIfStatement : public StatementObj {
public:
	shared_ptr<ExpressionObj> expr;
	std::vector<shared_ptr<StatementObj>> stmts;

	ElseIfStatement(shared_ptr<ExpressionObj> expr_arg,
		std::vector<shared_ptr<StatementObj>> stmts_arg) :
		StatementObj(ElseIfStatementType), expr(expr_arg),
		stmts(stmts_arg) {};
};

class IfStatement : public StatementObj {
public:
	shared_ptr<ExpressionObj> expr;
	std::vector<shared_ptr<StatementObj>> stmts;
	std::vector<shared_ptr<ElseIfStatement>> else_if_stmts;
	shared_ptr<ElseStatement> else_stmt;

	IfStatement(shared_ptr<ExpressionObj> expr_arg,
		std::vector<shared_ptr<StatementObj>> stmts_arg,
		std::vector<shared_ptr<ElseIfStatement>> else_if_stmts_arg,
		shared_ptr<ElseStatement> else_stmt_arg) : StatementObj(IfStatementType),
		expr(expr_arg), stmts(stmts_arg), else_if_stmts(else_if_stmts_arg),
		else_stmt(else_stmt_arg) {};
};

class ConditionJmpStatement : public StatementObj {
public:
	std::vector<shared_ptr<StatementObj>>::iterator jmp_stmt;
	shared_ptr<ExpressionObj> expr;

	ConditionJmpStatement(std::vector<shared_ptr<StatementObj>>::iterator jmp_stmt_arg,
		shared_ptr<ExpressionObj> expr_arg) : StatementObj(ConditionJmpType),
		jmp_stmt(jmp_stmt_arg), expr(expr_arg) {};

};

class ConditionActionJmpStatement : public StatementObj {
public:
	std::vector<shared_ptr<StatementObj>>::iterator jmp_stmt;
	shared_ptr<ExpressionObj> expr;
	shared_ptr<StatementObj> action;

	ConditionActionJmpStatement(std::vector<shared_ptr<StatementObj>>::iterator jmp_stmt_arg,
		shared_ptr<ExpressionObj> expr_arg, shared_ptr<StatementObj> action_arg) : StatementObj(ConditionActionJmpType),
		jmp_stmt(jmp_stmt_arg), expr(expr_arg), action(action_arg) {};

};

class WhileLoopStatement : public StatementObj {
public:
	shared_ptr<ExpressionObj> expr;
	std::vector<shared_ptr<StatementObj>> stmts;


	WhileLoopStatement(shared_ptr<ExpressionObj> expr_arg,
		std::vector<shared_ptr<StatementObj>> stmts_arg) :
		StatementObj(WhileLoopStatementType), expr(expr_arg),
		stmts(stmts_arg) {
		stmts.push_back(make_shared<ConditionJmpStatement>(stmts.begin(), expr));
		std::dynamic_pointer_cast<ConditionJmpStatement>(stmts.back())->jmp_stmt = stmts.begin();
	};
};

class ForLoopStatement : public StatementObj {
public:
	shared_ptr<VariableDecleration> var_decl;
	shared_ptr<ExpressionObj> expr;
	shared_ptr<StatementObj> stmt;
	std::vector<shared_ptr<StatementObj>> stmts;


	ForLoopStatement(shared_ptr<ExpressionObj> expr_arg,
		std::vector<shared_ptr<StatementObj>> stmts_arg,
		shared_ptr<StatementObj> stmt_arg,
		shared_ptr<VariableDecleration> var_decl_arg) :
		StatementObj(ForLoopStatementType), expr(expr_arg),
		stmts(stmts_arg), stmt(stmt_arg), var_decl(var_decl_arg) {
		stmts.push_back(make_shared<ConditionActionJmpStatement>(stmts.begin(), expr, stmt));
		std::dynamic_pointer_cast<ConditionActionJmpStatement>(stmts.back())->jmp_stmt = stmts.begin();
	
	};
};

class VariableReInitStatement : public StatementObj {
public:
	std::string name;
	shared_ptr<ExpressionObj> val;

	VariableReInitStatement(std::string var_name, shared_ptr<ExpressionObj> value) : StatementObj(
		VariableReInitStatementType),
		name(var_name), val(value) {};
};

class FunctionDecleration : public StatementObj {
public:
	std::string func_n;
	std::vector<shared_ptr<IdentifierExpr>> args;
	std::vector<shared_ptr<StatementObj>> stmts;

	FunctionDecleration(std::string name, std::vector<shared_ptr<IdentifierExpr>> args_arg,
		std::vector<shared_ptr<StatementObj>> stmts_arg) : StatementObj(FunctionDeclerationType),
		args(args_arg), stmts(stmts_arg), func_n(name) {};
};

class BreakStatement : public StatementObj {
public:
	BreakStatement() : StatementObj(BreakStatementType) {};
};

class ContinueStatement : public StatementObj {
public:
	ContinueStatement() : StatementObj(ContinueStatementType) {};
};

class ReturnStatement : public StatementObj {
public:
	shared_ptr<ExpressionObj> value;
	ReturnStatement(std::shared_ptr<ExpressionObj> val) : StatementObj(ReturnStatementType), value(val) {};
};

class ThrowStatement : public StatementObj {
public:
	Error err;

	ThrowStatement(Error error) : StatementObj(ThrowStatementType), err(error) {};
};


class CatchStatement : public StatementObj {
public:
	shared_ptr<IdentifierExpr> arg_name;
	std::vector<shared_ptr<StatementObj>> stmts;

	CatchStatement(std::vector<shared_ptr<StatementObj>> stmts_arg,
		shared_ptr<IdentifierExpr> arg_name_arg) :
		StatementObj(CatchStatementType), stmts(stmts_arg), arg_name(arg_name_arg) {};
};

class TryStatement : public StatementObj {
public:
	std::vector<shared_ptr<StatementObj>> stmts;
	shared_ptr<CatchStatement> catch_stmt;

	TryStatement(std::vector<shared_ptr<StatementObj>> stmts_arg,
		shared_ptr<CatchStatement> catch_stmt_arg) :
		StatementObj(TryStatementType), stmts(stmts_arg), catch_stmt(catch_stmt_arg) {};
};

class ArrayExpr : public ExpressionObj {
public:
	std::vector<shared_ptr<ExpressionObj>> elms;

	ArrayExpr(std::vector<shared_ptr<ExpressionObj>> elements): ExpressionObj(ArrayExpressionType), elms(elements) {};
};

class IndexAccessExpr : public ExpressionObj {
public:
	shared_ptr<ExpressionObj> array_expr;
	std::vector<shared_ptr<ExpressionObj>> index_path;

	IndexAccessExpr(std::vector<shared_ptr<ExpressionObj>> index_arg, 
		shared_ptr<ExpressionObj> array_expression): ExpressionObj(IndexAccessExpressionType),
		index_path(index_arg), array_expr(array_expression) {};
};

class LambdaExpression : public ExpressionObj {
public:
	std::vector<std::string> args;
	std::vector<std::shared_ptr<StatementObj>> stmts;
	
	LambdaExpression(std::vector<std::string> args_arg, 
		std::vector<std::shared_ptr<StatementObj>> stmts_arg):
		ExpressionObj(LambdaExprType), args(args_arg), stmts(stmts_arg){};

};

class IndexReInitStmt : public StatementObj {
public:
	std::shared_ptr<ExpressionObj> var_val;
	std::vector<shared_ptr<ExpressionObj>> index_path;
	std::shared_ptr<ExpressionObj> val;

	IndexReInitStmt(std::vector<shared_ptr<ExpressionObj>> index_arg, 
		std::shared_ptr<ExpressionObj> variable_val, std::shared_ptr<ExpressionObj> new_val): StatementObj(IndexReinitType),
		index_path(index_arg), var_val(variable_val), val(new_val) {};
};

class StructDecleration : public StatementObj{
public:
	std::string name;
	std::vector<std::string> props;
	
	StructDecleration(std::string struc_name,
		std::vector<std::string> struc_props_names): StatementObj(StructDeclerationType),
		name(struc_name), props(struc_props_names) {};
};