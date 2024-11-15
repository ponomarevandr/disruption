#pragma once

#include "matheval.h"

#include <vector>
#include <string>
#include <map>


namespace Evaluation {

class Formula {
private:
	std::string text;
	mutable void* evaluator = nullptr;
	std::vector<std::string> dependencies;

public:
	Formula() = default;
	explicit Formula(const std::string& text);
	Formula(Formula&&);
	Formula& operator=(Formula&&);
	~Formula();
	void prepare();
	const std::string& getText() const;
	const std::vector<std::string>& getDependencies() const;
	double evaluate(const std::map<std::string, double>& variables) const;
};

}