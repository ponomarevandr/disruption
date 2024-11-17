#pragma once

#include "model/model_parameters.h"
#include "display/progress_bar.h"

#include <iostream>
#include <vector>
#include <string>


class Model {
public:
	Model(ModelParameters&& params, std::ostream& out, const std::string& description =
		"A one-dimensional setting of the electrical breakdown problem");
	void run();

protected:
	ModelParameters params;
	std::ostream& out;
	std::string description;
	std::vector<double> phi;
	std::vector<double> phi_t;
	std::vector<double> phi_x_x;
	ProgressBar progress_bar;

protected:
	void initialize();
	double f(double phi) const;
	double f_phi(double phi) const;
	double eps_phi(size_t i) const;
	void iterationDerivatives();
	void iterationUpdate();
	void printValues(std::ostream& out) const;
};