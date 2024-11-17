#pragma once

#include "evaluation/environment.h"

#include <iostream>
#include <functional>


struct ModelParameters {
	std::vector<double> eps_0;
	double delta;
	double l;
	double Gamma;
	double m;
	double Phi_gradient;
	size_t x_grid;
	size_t x_skip;
	size_t t_grid;
	size_t t_skip;
	double width;
	double duration;
	std::vector<double> phi_0;
	double dx;
	double dt;
	Evaluation::Environment environment;

	void read(std::istream& in);
	void write(std::ostream& out) const;
	void initializeArrays();
};