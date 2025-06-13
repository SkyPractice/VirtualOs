#pragma once
#include "MhmRt.h"
#include <unordered_map>
#include <unordered_set>

using std::shared_ptr;

class Scope : public std::enable_shared_from_this<Scope> {
public:
	shared_ptr<Scope> parent_scope;
	std::unordered_map<std::string, shared_ptr<RunTimeVal>> variables;
	std::vector<std::shared_ptr<StatementObj>>::iterator c_stmt;
	std::vector<std::shared_ptr<StatementObj>>::iterator c_stmt_end;
	std::unordered_set<std::string> no_del_vars;

	Scope(shared_ptr<Scope> parent) : parent_scope(parent) {};

	shared_ptr<Scope> popUp(std::string var_name) {
		const auto var_itr = variables.find(var_name);
		if (var_itr != variables.end())
			return shared_from_this();
		else if (parent_scope == nullptr) return nullptr;
		else return parent_scope->popUp(var_name);

	}

	shared_ptr<RunTimeVal> lookUpVar(std::string name) {
		shared_ptr<Scope> scope = popUp(name);
		if (scope == nullptr) throw std::exception("Variable Not Found");
		return scope->variables[name];
	}

	shared_ptr<RunTimeVal>& lookUpVarByRef(std::string name) {
		shared_ptr<Scope> scope = popUp(name);
		if (scope == nullptr) throw std::exception("Variable Not Found");
		return scope->variables[name];
	}

	void setVar(std::string var_name, shared_ptr<RunTimeVal> val) {
		shared_ptr<Scope> scope = popUp(var_name);
		if (scope == nullptr) throw std::exception("Variable Not Found");
		scope->variables[var_name] = val;
	}

	void addVar(std::string name, shared_ptr<RunTimeVal> val) {
		if (variables.find(name) != variables.end())
			throw std::exception("variable already exists");
		variables.insert({ name, val });
	}


};