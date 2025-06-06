#include "Parser.h"

std::vector<Token>::iterator Parser::token;

shared_ptr<ProgramObj> Parser::produceAst(std::vector<Token> tokens) {
	std::vector<shared_ptr<StatementObj>> stmts;

	token = tokens.begin();

	while (token->type != EndOfFile) {
		auto stat = parseStatement();
		stmts.push_back(stat);
		switch (stat->type)
		{
		case If:
		{
			auto if_stmt = std::dynamic_pointer_cast<IfStatement>(*(stmts.end() - 1));
			for (auto& st : if_stmt->stmts) {
				st->p_itr = stmts.end() - 1;
			}
			for (auto st = if_stmt->else_if_stmts.begin(); st != if_stmt->else_if_stmts.end(); st++) {
				(*st)->p_itr = stmts.end() - 1;
				for (auto& statem : (*st)->stmts) {
					statem->p_itr = stmts.end() - 1;
				}
			}
			for (auto& st : if_stmt->else_stmt->stmts) {
				st->p_itr = stmts.end() - 1;
			}
			break;
		}
		default:
			break;
		}

	}

	return make_shared<ProgramObj>(stmts);	
}


shared_ptr<StatementObj> Parser::parseStatement() {
	switch (token->type)
	{
	case Variable:
		return parseVariableDecleration();
	case If:
		return parseIfStatement();
	case While:
		return parseWhileLoop();
	case For:
		return parseForLoop();
	case ReInit:
		return parseVariableInit();
	case Func:
		return parseFunctionDecleration();
	case Return:
		return parseReturnStatement();
	case Break:
		advance();
		return make_shared<BreakStatement>();
	case Continue:
		advance();
		return make_shared<ContinueStatement>();
	case Try:
		return parseTryStatement();
	case Throw:
		return parseThrowStatement();
	default:
		return parseExpression();
		break;
	}
};

shared_ptr<ExpressionObj> Parser::parseExpression() {
	return parseArrayExpression();
};
shared_ptr<ExpressionObj> Parser::parseAdditiveExpression() {
	auto left = parseMultiplicativeExpression();

	while (token->symbol == "+" || token->symbol == "-") {
		const std::string op = advance().symbol;
		const auto right = parseMultiplicativeExpression();
		left = make_shared<BinaryExpressionObj>(left, right, op);
	}

	return left;
};
shared_ptr<ExpressionObj> Parser::parseMultiplicativeExpression() {
	auto left = parsePrimaryExpression();

	while (token->symbol == "*" || token->symbol == "/" || token->symbol == "%") {
		const std::string op = advance().symbol;
		const auto right = parsePrimaryExpression();
		left = make_shared<BinaryExpressionObj>(left, right, op);
	}

	return left;
};

shared_ptr<ExpressionObj> Parser::parseComparativeExpression() {
	auto left = parseAdditiveExpression();

	while (token->symbol == ">" || token->symbol == "<" || token->symbol == "==" || token->symbol == "=>" ||
		token->symbol == "=<" || token->symbol == "=!") {
		const std::string op = advance().symbol;
		const auto right = parseAdditiveExpression();
		left = make_shared<BinaryExpressionObj>(left, right, op);
	}

	return left;
}


shared_ptr<ExpressionObj> Parser::parsePrimaryExpression() {
	switch (token->type)
	{
	case Call:
		return parseFunctionCallExpression();
	case String:
		return parseStringExpression();
	case True:
	case False:
		return parseBooleanExpression();
	case NumuericLiteral:
		return make_shared<NumExpression>(stoi(advance().symbol));
	case OpenParen: {
		advance();
		auto ret = parseExpression();
		advance();
		return ret;
	}
	case Identifier:
		return make_shared<IdentifierExpr>(advance().symbol);
	default:
		throw std::exception("Unexcepted Token");
		break;
	}
};

shared_ptr<ExpressionObj> Parser::parseStringExpression() { 

	return make_shared<StringExpression>(advance().symbol);
};
shared_ptr<ExpressionObj> Parser::parseFunctionCallExpression() { 
	
	std::vector<shared_ptr<ExpressionObj>> args;

	const Token call_indicator = advance();
	const Token func_name = advance();
	const Token open_paren = advance();

	while (token->type != CloseParen) {
		args.push_back(parseExpression());
		if (token->type == Comma)
			advance();
		else break; // close parenthes
	}

	const Token closing_paren = advance();

	return make_shared<FunctionCall>(make_shared<IdentifierExpr>(func_name.symbol), args);
};
shared_ptr<ExpressionObj> Parser::parseBooleanExpression() { 
	const Token expr = advance();

	if (expr.type == True)
		return make_shared<BooleanExpression>(true);
	else return make_shared<BooleanExpression>(false);
};

shared_ptr<VariableDecleration> Parser::parseVariableDecleration() {

	const Token var_word = advance();
	const Token iden = advance();
	const Token equal = advance();
	const auto expr = parseExpression();
	
	return make_shared<VariableDecleration>(iden.symbol, expr);
};

shared_ptr<IfStatement> Parser::parseIfStatement() {
	advance(); // through the if word

	const auto expr = parseExpression(); // expr
	const Token open_brace = advance();
	std::vector<shared_ptr<StatementObj>> stmts;

	while (token->type != CloseBrace) {
		stmts.push_back(parseStatement());
	}

	const Token close_brace = advance();
	std::vector<shared_ptr<ElseIfStatement>> else_if_stmts;

	while (token->type == ElseIf) {
		const Token else_if = advance();
		const auto expression = parseExpression(); // expr

		const Token open_b = advance();

		std::vector<shared_ptr<StatementObj>> else_if_stmts_objs;

		while (token->type != CloseBrace) {
			else_if_stmts_objs.push_back(parseStatement());
		}

		const Token close_b = advance();
		
		else_if_stmts.push_back(std::make_shared<ElseIfStatement>(expression, else_if_stmts_objs));

	}

	if (token->type != Else) return make_shared<IfStatement>(expr, stmts, else_if_stmts, nullptr);

	const Token else_word = advance();
	const Token open_else_brace = advance();

	std::vector<shared_ptr<StatementObj>> else_stmts;
	while (token->type != CloseBrace) {
		else_stmts.push_back(parseStatement());
	}

	const Token else_close_b = advance();

	shared_ptr<ElseStatement> else_stmt = make_shared<ElseStatement>(else_stmts);

	return make_shared<IfStatement>(expr, stmts, else_if_stmts, else_stmt);

}
shared_ptr<WhileLoopStatement> Parser::parseWhileLoop() {

	advance(); // through the while word

	const auto expr = parseExpression(); // expression
	const Token open_brace = advance();
	std::vector<shared_ptr<StatementObj>> stmts;

	while (token->type != CloseBrace) {
		stmts.push_back(parseStatement());
	}
	const Token close_brace = advance();

	return make_shared<WhileLoopStatement>(expr, stmts);
}
shared_ptr<ForLoopStatement> Parser::parseForLoop() {
	
	const Token for_l_word = advance();
	const auto var_decl = parseVariableDecleration();
	advance(); // ,
	const auto expr = parseExpression();
	advance(); // ,
	const auto reinit = parseStatement();
	advance(); // {

	std::vector<shared_ptr<StatementObj>> stmts;

	while (token->type != CloseBrace) {
		stmts.push_back(parseStatement());
	}

	advance(); // }

	return make_shared<ForLoopStatement>(expr, stmts, reinit, var_decl);
}
shared_ptr<VariableReInitStatement> Parser::parseVariableInit() {

	const Token reinit_word = advance();
	const Token var = advance();
	const Token eq = advance();
	const auto expr = parseExpression();

	return make_shared<VariableReInitStatement>(var.symbol, expr);

}
shared_ptr<FunctionDecleration> Parser::parseFunctionDecleration() {
	const Token func_word = advance();

	const Token func_name = advance();

	const Token open_paren = advance();

	std::vector<shared_ptr<IdentifierExpr>> args;

	while (token->type != CloseParen) {
		args.push_back(make_shared<IdentifierExpr>(advance().symbol));
		if (token->type == Comma)
			advance();
		else break; // close parenthes
	}

	const Token closing_paren = advance();

	advance(); // {

	std::vector<shared_ptr<StatementObj>> stmts;

	while (token->type != CloseBrace) {
		stmts.push_back(parseStatement());
	}

	advance(); // }

	return make_shared<FunctionDecleration>(func_name.symbol, args, stmts);

}
shared_ptr<ReturnStatement> Parser::parseReturnStatement() {

	const Token ret_word = advance();
	const auto expr = parseExpression();

	return make_shared<ReturnStatement>(expr);

}
shared_ptr<TryStatement> Parser::parseTryStatement() {
	advance(); // eat try keyword

	const Token open_brace = advance();
	std::vector<shared_ptr<StatementObj>> stmts;

	while (token->type != CloseBrace) {
		stmts.push_back(parseStatement());
	}

	const Token close_brace = advance();

	if (token->type != Catch) return make_shared<TryStatement>(stmts, nullptr);
	const Token catch_word = advance();

	const Token excep_name = advance();

	advance(); // {

	std::vector<shared_ptr<StatementObj>> stmts_catch;

	while (token->type != CloseBrace) {
		stmts_catch.push_back(parseStatement());
	}

	advance(); // }

	return make_shared<TryStatement>(stmts,
		make_shared<CatchStatement>(stmts_catch, make_shared<IdentifierExpr>(excep_name.symbol)));

}
shared_ptr<ThrowStatement> Parser::parseThrowStatement() {
	const Token word = advance();
	auto expr = parseExpression();
	const Error err = { expr, RunTimeException };
	return make_shared<ThrowStatement>(err);
}

shared_ptr<ExpressionObj> Parser::parseArrayExpression(){
	if (token->type == OpenBracket)
	{
		const Token open_bracket = advance();
		std::vector<shared_ptr<ExpressionObj>> exprs;

		while (token->type != CloseBracket)
		{
			exprs.push_back(parseExpression());
			if (token->type == Comma)
			{
				advance();
			}
		}

		const Token close_bracket = advance();

		return make_shared<ArrayExpr>(exprs);
	}

	return parseIndexAccessExpression();

};

shared_ptr<ExpressionObj> Parser::parseIndexAccessExpression()
{
	if (token->type == Identifier && (token + 1)->type == OpenBracket)
	{
		const Token var_name = advance();
		std::vector<std::shared_ptr<ExpressionObj>> path;
		while(token->type == OpenBracket){
			advance();
			path.push_back(parseExpression());
			advance();
		}

		return make_shared<IndexAccessExpr>(path, var_name.symbol);
	}

	return parseComparativeExpression();
}