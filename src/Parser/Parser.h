#pragma once
#include "../Lexer/Lexer.h"
#include "Ast.h"

class Parser {
public:
	static std::vector<Token>::iterator token;
	
	static shared_ptr<ProgramObj> produceAst(std::vector<Token> tokens);

	static shared_ptr<StatementObj> parseStatement();

	static shared_ptr<ExpressionObj> parseExpression();


	static shared_ptr<ExpressionObj> parseAdditiveExpression();
	static shared_ptr<ExpressionObj> parseMultiplicativeExpression();
	static shared_ptr<ExpressionObj> parseComparativeExpression();

	static shared_ptr<ExpressionObj> parseStringExpression();
	static shared_ptr<ExpressionObj> parseFunctionCallExpression();
	static shared_ptr<ExpressionObj> parseArrayExpression();
	static shared_ptr<ExpressionObj> parseBooleanExpression();
	static shared_ptr<ExpressionObj> parseIndexAccessExpression();
	static shared_ptr<ExpressionObj> parseLambdaExpression();
	 
	static shared_ptr<VariableDecleration> parseVariableDecleration();
	static shared_ptr<IfStatement> parseIfStatement(); // includes elseif and else
	static shared_ptr<WhileLoopStatement> parseWhileLoop();
	static shared_ptr<ForLoopStatement> parseForLoop();
	static shared_ptr<VariableReInitStatement> parseVariableInit();
	static shared_ptr<FunctionDecleration> parseFunctionDecleration();
	static shared_ptr<ReturnStatement> parseReturnStatement();
	static shared_ptr<TryStatement> parseTryStatement(); // includes catch
	static shared_ptr<ThrowStatement> parseThrowStatement();
	static shared_ptr<IndexReInitStmt> parseIndexReinit();


	static shared_ptr<ExpressionObj> parsePrimaryExpression();

	static Token advance() {
		if (token->type != EndOfFile) {
			Token t = *token;
			token++;
			return t;
		}
		return *token;
	}
};