#pragma once

#include "evaluation/formula.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>


namespace Evaluation {

struct Function {
	struct Part {
		Formula bound_left;
		Formula bound_right;
		Formula formula;

		static Part read(std::istream&);
		void write(std::ostream&) const;
	};

	std::vector<Part> parts;

	static Function read(std::istream&);
	void write(std::ostream&) const;
};


class Environment {
private:
	std::map<std::string, double> variables;
	std::map<std::string, double> rewritten;
	std::map<std::string, Function> functions;
	std::map<std::string, Formula> variable_formulas;

public:
	Environment() = default;
	void read(std::istream&);
	void write(std::ostream&);
	
};

}