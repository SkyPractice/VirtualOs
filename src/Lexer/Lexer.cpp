#include "Lexer.h"

char Lexer::ch = '\0';
int Lexer::idx = 0;
std::string Lexer::source_code = "";
std::string Lexer::skip_str = "\t \n\r	";
std::unordered_map<std::string, TokenAst> Lexer::known_tokens = {
	{ "(", OpenParen }, { ")", CloseParen } , { "{", OpenBrace }, { "}", CloseBrace },
	{ "[", OpenBracket }, { "]", CloseBracket },
	{ "+", BinaryOperator }, { "-", BinaryOperator }, { "*", BinaryOperator }, { "/", BinaryOperator },
	{ "%", BinaryOperator }, { "variable", Variable }, { "func", Func }, { "=", Equal },
	{ "true", True }, { "false", False }, { "call", Call }, { ",", Comma }, { "\"", DoubleQuote },
	{ "if", If}, { "elseif", ElseIf}, { "else", Else }, { "while", While }, { "for", For },
	{ "class", Class }, { ">", BinaryOperator }, { "<", BinaryOperator }, { "throw", Throw },
	{ "catch", Catch }, { "try", Try }, { "reinit", ReInit }, { "return", Return }, { "break", Break },
	{ "continue", Continue }, {"lambda", Lambda},{"idx", IndexAccess},  { "idx_reinit", IndexReinit },
	{ "struct", Struct }
};

std::vector<Token> Lexer::tokenize(std::string source_path) {

	getSourceFromPath(source_path);

	std::vector<Token> tokens;

	idx = 0;
	ch = source_code[0];

	for (;idx < source_code.size() && ch != '\0';) {
		while (skip_str.find(ch) != std::string::npos && ch != '\0')
			advance();

		if (isdigit(ch) || (ch == '-' && isdigit(source_code[idx + 1]))) {
			std::string num = "";

			while (isdigit(ch) || (ch == '-' && isdigit(source_code[idx + 1]))) {
				num.push_back(ch);
				advance();
			}

			tokens.push_back({ num, NumuericLiteral });
		}
		else if (isalpha(ch)) {
			std::string iden = "";

			while (isalpha(ch) || ch == '_' || isdigit(ch)) {
				iden.push_back(ch);
				advance();
			}

			const auto find_itr = known_tokens.find(iden);

			if (find_itr == known_tokens.end())
				tokens.push_back({ iden, Identifier });
			else tokens.push_back({ iden, find_itr->second });

		}
		else {
			if (ch == '=' && (source_code[idx + 1] == '=' || source_code[idx + 1] == '>' || source_code[idx + 1] == '<' ||
				source_code[idx + 1] == '!'
				)) {
				std::string th = "";
				th.push_back(ch);
				advance();
				th.push_back(ch);
				advance();
				tokens.push_back({ th, BinaryOperator });
				continue;
			}
			else if (ch == '\"') {
				std::string str_s = "";
				advance();
				while (ch != '\"') {
					if(ch == '\\' && source_code[idx + 1] == 'n'){
						str_s.push_back('\n');
						advance();
					}
					else str_s.push_back(ch);
					advance();
				}
				advance();
				tokens.push_back({ str_s, String });
				continue;
			}
				
			std::string thing = "";
			thing.push_back(ch);
			const auto itr = known_tokens.find(thing);
			if (itr != known_tokens.end())
				tokens.push_back({ thing, itr->second });
			else throw std::exception("Unknown Token");
			advance();

		}
	}

	ch = '\0';
	idx = 0;
	source_code = "";

	tokens.push_back({ "\0", EndOfFile });

	return tokens;
}
void Lexer::advance() {

	if (ch != '\0' && idx < source_code.size()) {
		ch = source_code[++idx];
	}
	else ch = '\0';
}

void Lexer::getSourceFromPath(std::string path) {

	if (!fs::exists(path))
		throw std::exception("File Doesn't Exist");
	
	std::ifstream file_stream(path);

	file_stream.seekg(0, std::ios::end);

	const int size = file_stream.tellg();

	file_stream.seekg(0, std::ios::beg);

	source_code.resize(size);
	
	file_stream.read(&source_code[0], size);

	file_stream.close();

}
