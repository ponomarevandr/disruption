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
		in.get();
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
	out << "\t[" << bound_left.getText() << "," << bound_right.getText() << "]:";
	out << formula.getText() << "\n";
}

Function Function::read(std::istream& in) {
	Function result;
	ignoreSpace(in);
	do {
		result.parts.push_back(Part::read(in));
		ignoreSpace(in);
	} while (in.peek() == '[');
	return result;
}

void Function::write(std::ostream& out) const {
	for (const Part& part : parts) {
		part.write(out);
	}
}

double Function::getValue(const std::map<std::string, double>& variables) const {
	for (const Part& part : parts) {
		double bound_left_value = part.bound_left.evaluate(variables);
		double bound_right_value = part.bound_right.evaluate(variables);
		if (bound_left_value <= variables.at("x") && variables.at("x") <= bound_right_value)
			return part.formula.evaluate(variables);
	}
	throw std::invalid_argument(
		"The function in not defined for x = " + std::to_string(variables.at("x")) + "!"
	);
}


void Environment::read(std::istream& in) {
	variables.clear();
	functions.clear();
	variable_formulas.clear();
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
			Function function = Function::read(in);
			if (functions.count(name))
				throw std::invalid_argument("Redefenition of \"" + name + "()\"!");
			functions.insert(std::make_pair(std::move(name), std::move(function)));
		} else {
			if (name == "x")
				throw std::invalid_argument("The name \"x\" is not allowed!");
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

void Environment::write(std::ostream& out) const {
	for (const auto& element : variable_formulas) {
		out << element.first << " =" << element.second.getText() << "\n";
	}
	for (const auto& element : functions) {
		out << element.first << "() =\n";
		element.second.write(out);
	}
}

double Environment::getVariableValue(const std::string& name) const {
	auto iterator = variables.find(name);
	if (iterator == variables.end())
		throw std::invalid_argument("The variable \"" + name + "\" is undefined!");
	return iterator->second;
}

double Environment::getFunctionValue(const std::string& name, double argument) const {
	auto iterator = functions.find(name);
	if (iterator == functions.end())
		throw std::invalid_argument("The function \"" + name + "()\" is undefined!");
	variables.insert(std::make_pair("x", argument));
	double result = iterator->second.getValue(variables);
	variables.erase("x");
	return result;
}

void Environment::debugPrint(std::ostream& out) const {
	out << "========== Evaluation environment\n";
	out << "* Variables:\n";
	for (const auto& element : variables) {
		out << element.first << " = " << element.second << ":\t";
		out << variable_formulas.at(element.first).getText() << "\n";
	}
	out << "* Functions:\n";
	for (const auto& element : functions) {
		out << element.first << "() =\n";
		element.second.write(out);
	}
	out << "==========\n";
}

}