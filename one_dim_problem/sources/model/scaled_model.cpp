#include "scaled_model.h"

#include <cmath>
#include <algorithm>


ScaledModel::ScaledModel(const ModelParameters& params, std::ostream& out,
		const std::string& description): params(params), out(out), description(description) {
	dx = params.width / params.x_grid;
	dt = params.duration / params.t_grid;
}

void ScaledModel::run() {
	out << description << "\n";
	params.print(out);
	initialize();
	printValues(out);
	for (size_t i = 1; i <= params.t_grid; i += params.t_skip) {
		for (size_t j = 0; j < params.t_skip; ++j) {
			iteration();
		}
		printValues(out);
	}
}

double ScaledModel::phi_0(double x) const {
	const double PI = 3.14159265358979324;
	if (x < params.l)
		return 0.5 - 0.5 * std::cos(x / params.l * PI);
	return 1.0;
}

void ScaledModel::initialize() {
	phi.resize(params.x_grid + 1);
	for (size_t i = 0; i <= params.x_grid; ++i) {
		phi[i] = phi_0(i * dx);
	}
	phi_next.resize(params.x_grid + 1);
}

double ScaledModel::f(double phi) const {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double ScaledModel::f_hatch(double phi) const {
	return 12.0 * phi * phi * (1.0 - phi);
}

void ScaledModel::iteration() { 
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		double phi_partial_x = 0.5 * (phi[i + 1] - phi[i - 1]) / dx;
		double phi_partial_x_x = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) / (dx * dx);
		double phi_partial_t = params.m * (params.Gamma / (params.l * params.l) * f_hatch(phi[i]) +
			params.Gamma * (phi_partial_x + i * dx * phi_partial_x_x));
		phi_next[i] = phi[i] + dt * phi_partial_t;
	}
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi[i] = phi_next[i];
	}
}

void ScaledModel::printValues(std::ostream& out) const {
	for (size_t i = 0; i <= params.x_grid; i += params.x_skip) {
		out << phi[i];
		if (i + params.x_skip <= params.x_grid)
			out << ";";
	}
	out << "\n";
}