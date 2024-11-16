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
	double getValue(const std::map<std::string, double>& variables) const;
};


class Environment {
private:
	mutable std::map<std::string, double> variables;
	std::map<std::string, Function> functions;
	std::map<std::string, Formula> variable_formulas;

public:
	Environment() = default;
	void read(std::istream&);
	void write(std::ostream&) const;
	double getVariableValue(const std::string& name) const;
	double getFunctionValue(const std::string& name, double argument) const;
	void debugPrint(std::ostream&) const;
};

}