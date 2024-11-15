#include "environment.h"

#include <sstream>
#include <stdexcept>


namespace Evaluation {

std::string readUntil(std::istream& in, char stop_symbol) {
	std::string result;
	char current;
	while (in.get(current) && current != stop_symbol) {
		result.push_back(current);
	}
	return result;
}

void ignoreSpace(std::istream& in) {
	while (!in.eof()) {
		char current = in.peek();
		if (!std::isspace(current))
			break;
		in.get();
	}
}


Function::Part Function::Part::read(std::istream& in) {
	Part result;
	if (in.peek() == '[') {
		result.bound_left = Formula(readUntil(in, ','));
		result.bound_right = Formula(readUntil(in, ']'));
		in.get();
	} else {
		std::stringstream sstream("-inf, inf]");
		result.bound_left = Formula(readUntil(sstream, ','));
		result.bound_right = Formula(readUntil(sstream, ']'));
	}
	result.bound_left.prepare();
	result.bound_right.prepare();
	result.formula = Formula(readUntil(in, '\n'));
	result.formula.prepare();
	return result;
}

void Function::Part::write(std::ostream& out) const {
	out << "[" << bound_left.getText() << "," << bound_right.getText() << "]:";
	out << formula.getText() << "\n";
}

Function Function::read(std::istream& in) {
	Function result;
	ignoreSpace(in);
	do {
		result.parts.push_back(Part::read(in));
	} while (in.peek() == '[');
	return result;
}

void Function::write(std::ostream& out) const {
	for (const Part& part : parts) {
		part.write(out);
	}
}


void Environment::read(std::istream& in) {
	variables.clear();
	rewritten.clear();
	functions.clear();
	while (true) {
		ignoreSpace(in);
		if (in.eof())
			break;
		std::string name = readUntil(in, '=');
		while (std::isspace(name.back())) {
			name.pop_back();
		}
		if (name.back() == ')') {
			name.pop_back();
			name.pop_back();
			readUntil(in, '\n');
			Function function = Function::read(in);
			if (functions.count(name))
				throw std::invalid_argument("Redefenition of \"" + name + "\"()!");
			functions.insert(std::make_pair(std::move(name), std::move(function)));
		} else {
			Formula formula(readUntil(in, '\n'));
			formula.prepare();
			double value = formula.evaluate(variables);
			if (variables.count(name))
				throw std::invalid_argument("Redefenition of \"" + name + "\"!");
			variables.insert(std::make_pair(name, value));
			variable_formulas.insert(std::make_pair(std::move(name), std::move(formula)));
		}
	}
}

void Environment::write(std::ostream& out) {
	for (const auto& element : variable_formulas) {
		out << element.first << " =" << element.second.getText() << "\n";
	}
	for (const auto& element : functions) {
		out << element.first << "() = \n";
		element.second.write(out);
	}
}

}