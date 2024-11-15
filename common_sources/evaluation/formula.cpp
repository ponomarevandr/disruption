#include "formula.h"

#include <stdexcept>


namespace Evaluation {

Formula::Formula(const std::string& text): text(text) {}

Formula::Formula(Formula&& other): text(std::move(other.text)), evaluator(other.evaluator),
		dependencies(std::move(other.dependencies)) {
	other.evaluator = nullptr;
}

Formula& Formula::operator=(Formula&& other) {
	text = std::move(other.text);
	if (evaluator)
		evaluator_destroy(evaluator);
	evaluator = other.evaluator;
	other.evaluator = nullptr;
	dependencies = std::move(other.dependencies);
	return *this;
}

Formula::~Formula() {
	if (evaluator)
		evaluator_destroy(evaluator);
}

void Formula::prepare() {
	if (evaluator)
		return;
	evaluator = evaluator_create(const_cast<char*>(text.c_str()));
	if (!evaluator)
		throw std::invalid_argument("Invalid formula \"" + text + "\"!");
	char** names;
	int count;
	evaluator_get_variables(evaluator, &names, &count);
	for (int i = 0; i < count; ++i) {
		dependencies.push_back(names[i]);
	}
}

const std::string& Formula::getText() const {
	return text;
}

const std::vector<std::string>& Formula::getDependencies() const {
	return dependencies;
}

double Formula::evaluate(const std::map<std::string, double>& variables) const {
	std::vector<char*> names;
	std::vector<double> values;
	for (const std::string& name : dependencies) {
		auto iterator = variables.find(name);
		if (iterator == variables.end()) {
			throw std::invalid_argument(
				"Cannot evaluate \"" + text + "\": no value for \"" + name + "\"!"
			);
		}
		names.push_back(const_cast<char*>(iterator->first.c_str()));
		values.push_back(iterator->second);
	}
	return evaluator_evaluate(evaluator, names.size(), names.data(), values.data());
}

}