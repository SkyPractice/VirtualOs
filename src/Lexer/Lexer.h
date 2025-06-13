#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

enum TokenAst {
	NumuericLiteral, Identifier, BinaryOperator, 
	OpenParen, CloseParen, OpenBrace, CloseBrace,
	OpenBracket, CloseBracket, Equal, DoubleQuote, SingleQuote,
	Variable, EndOfFile, Func, True, False, Call, Comma, If, ElseIf, Else,
	While, For, Class, Try, Catch, Throw, ReInit, Return, Break, Continue,
	String, Lambda, IndexReinit, IndexAccess
};

struct Token {
	std::string symbol;
	TokenAst type;
};

class Lexer {
public:
	static char ch;
	static int idx;
	static std::string source_code;
	static std::string skip_str;
	static std::unordered_map<std::string, TokenAst> known_tokens;

	static std::vector<Token> tokenize(std::string source_path);
	static void getSourceFromPath(std::string path);
	static void advance();

};