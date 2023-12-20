#pragma once

#include "model/model_parameters.h"

#include <iostream>
#include <vector>
#include <string>


class Model {
public:
	Model(const ModelParameters& params, std::ostream& out, const std::string& description =
		"A one-dimensional setting of the electrical breakdown problem");
	void run();

private:
	ModelParameters params;
	std::ostream& out;
	std::string description;
	double dx;
	double dt;
	std::vector<double> phi;
	std::vector<double> phi_next;
	static constexpr double START_DENT_WIDTH = 1.0;
	static constexpr double START_DENT_DEPTH = 0.05;

private:
	double phi_0(double x) const;
	void initialize();
	double f(double phi) const;
	double f_hatch(double phi) const;
	double eps_hatch(double phi) const;
	void iteration();
	void printValues(std::ostream& out) const;
};