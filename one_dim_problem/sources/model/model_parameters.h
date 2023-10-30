#pragma once

#include <iostream>


struct ModelParameters {
	double eps_0;
	double delta;
	double l;
	double Gamma;
	double m;
	double Phi_coefficient;
	double dx;
	size_t x_skip;
	double dt;
	size_t t_skip;
	double width;
	double duration;

	void setPhysical(double eps_0, double delta, double l, double Gamma, double m,
		double Phi_coefficient);
	void setComputational(double dx, size_t x_skip, double dt, size_t t_skip);
	void setSizes(double width, double duration);
	void print(std::ostream& out) const;
};