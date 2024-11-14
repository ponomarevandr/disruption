#pragma once

#include "model/model_parameters.h"
#include "display/progress_bar.h"

#include <iostream>
#include <vector>
#include <string>


class Model {
public:
	Model(const ModelParameters& params, std::ostream& out, const std::string& description =
		"A one-dimensional setting of the electrical breakdown problem");
	void run();

protected:
	ModelParameters params;
	std::ostream& out;
	std::string description;
	double dx;
	double dt;
	std::vector<double> phi;
	std::vector<double> phi_t;
	std::vector<double> phi_x_x;
	static constexpr double START_DENT_WIDTH = 0.5;
	static constexpr double START_DENT_DEPTH = 0.05;
	ProgressBar progress_bar;

protected:
	double phi_0(double x) const;
	void initialize();
	double f(double phi) const;
	double f_phi(double phi) const;
	double eps_phi(double phi) const;
	void iterationDerivatives();
	void iterationUpdate();
	void printValues(std::ostream& out) const;
};