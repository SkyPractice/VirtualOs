#pragma once
#include "../Parser/Parser.h"
#include "Scope.h"
#include "MhmRt.h"
#include "../Kernel/Interrupt.h"
#include <functional>
#include <unordered_set>

class Process;

class Interpreter {
public:
	std::vector<shared_ptr<StatementObj>> stmts;
	shared_ptr<Scope> program_scope;
	shared_ptr<Scope> current_scope;
	std::shared_ptr<StatementObj> stmt;
	Process* proc;
	bool switched = false;
	bool try_mode = false;
	std::unordered_map<std::string, std::shared_ptr<StructDecleration>> struct_decls;

	Interpreter(std::vector<shared_ptr<StatementObj>> statements) : stmts(statements),
		program_scope(std::make_shared<Scope>(nullptr)){
	
		current_scope = program_scope;
		current_scope->c_stmt = stmts.begin();
		current_scope->c_stmt_end = stmts.end();
		stmt = *current_scope->c_stmt;
	};
	
	std::shared_ptr<RunTimeVal> nextStatement();

	std::shared_ptr<RunTimeVal> evaluate(std::shared_ptr<StatementObj> statement);
	std::shared_ptr<RunTimeVal> evaluateBinaryExpr(std::shared_ptr<BinaryExpressionObj> expr);
	std::shared_ptr<RunTimeVal> evaluateNumericBinaryExpr(std::shared_ptr<NumVal> left, 
		std::shared_ptr<NumVal> right, std::string op);
	std::shared_ptr<RunTimeVal> evaluateBooleanBinaryExpr(std::shared_ptr<BoolVal> left,
		std::shared_ptr<BoolVal> right, std::string op);
	std::shared_ptr<RunTimeVal> evaluateVariableDecl(std::shared_ptr<VariableDecleration> decl);
	std::shared_ptr<RunTimeVal> evaluateIdentifier(std::shared_ptr<IdentifierExpr> iden);
	std::shared_ptr<RunTimeVal> evaluateIfStatement(std::shared_ptr<IfStatement> statement);
	std::shared_ptr<RunTimeVal> evaluateVariableReInit(std::shared_ptr<VariableReInitStatement> statement);
	std::shared_ptr<RunTimeVal> evaluateWhileLoop(std::shared_ptr<WhileLoopStatement> statement);
	std::shared_ptr<RunTimeVal> evaluateForLoop(std::shared_ptr<ForLoopStatement> statement);
	std::shared_ptr<RunTimeVal> evaluateTryStatement(std::shared_ptr<TryStatement> statement);
	std::shared_ptr<RunTimeVal> evaluateFuncDecl(std::shared_ptr<FunctionDecleration> statement);
	std::shared_ptr<RunTimeVal> evaluateThrow(std::shared_ptr<ThrowStatement> statement);
	std::shared_ptr<RunTimeVal> evaluateConditionJmp(std::shared_ptr<ConditionJmpStatement> statement);
	std::shared_ptr<RunTimeVal> evaluateConditionActionJmp(std::shared_ptr<ConditionActionJmpStatement> stmt);
	std::shared_ptr<RunTimeVal> evaluateBreakStatement();
	std::shared_ptr<RunTimeVal> evaluateContinueStatement();
	std::shared_ptr<RunTimeVal> evaluateArrayExpr(std::shared_ptr<ArrayExpr> expr);
	std::shared_ptr<RunTimeVal> evaluateIndexAccessExpr(std::shared_ptr<IndexAccessExpr> expr);
	std::shared_ptr<RunTimeVal> evaluateFunctionCall(std::shared_ptr<FunctionCall> call);
	std::shared_ptr<RunTimeVal> evaluateLambdaExpr(std::shared_ptr<LambdaExpression> expr);
	std::shared_ptr<RunTimeVal> evaluateIndexReInit(std::shared_ptr<IndexReInitStmt> stmt);
	std::shared_ptr<RunTimeVal> evaluateStringBinaryExpr(std::shared_ptr<StringVal> left, 
		std::shared_ptr<StringVal> right, std::string op);

};

class SystemCalls {
public:
	static std::unordered_map<std::string, std::function<shared_ptr<RunTimeVal>(std::vector<shared_ptr<RunTimeVal>>,
		Process*)>>  sys_calls;
	static std::unordered_map<std::string, std::function<shared_ptr<RunTimeVal>(std::vector<shared_ptr<RunTimeVal>>, Interpreter*)>>
		native_functions;
};