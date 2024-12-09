#pragma once

#include "evaluation/environment.h"

#include <iostream>
#include <vector>


struct ModelParameters {
	std::vector<double> eps_0;
	double delta;
	double l;
	std::vector<double> Gamma;
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
	double t_0 = 0;
	Evaluation::Environment environment;

	void read(std::istream& in, bool read_phi_0 = true);
	void write(std::ostream& out) const;
	void read_phi_0(std::istream& in);
};