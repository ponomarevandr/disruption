#include "model.h"

#include <cmath>
#include <algorithm>


Model::Model(const ModelParameters& params, std::ostream& out, const std::string& description):
		params(params), out(out), description(description) {
	dx = params.width / params.x_grid;
	dt = params.duration / params.t_grid;
}

void Model::run() {
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

double Model::phi_0(double x) const {
	x -= 0.5 * params.width;
	if (x <= -0.5 * START_DENT_WIDTH || 0.5 * START_DENT_WIDTH <= x)
		return 1.0;
	const double PI = 3.14159265358979324;
	return 1.0 - 0.5 * START_DENT_DEPTH * (1.0 + std::cos(x / START_DENT_WIDTH * 2.0 * PI));
}

void Model::initialize() {
	phi.resize(params.x_grid + 1);
	for (size_t i = 0; i <= params.x_grid; ++i) {
		phi[i] = phi_0(i * dx);
	}
	phi_next.resize(params.x_grid + 1);
}

double Model::f(double phi) const {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double Model::f_hatch(double phi) const {
	return 12.0 * phi * phi * (1.0 - phi);
}

double Model::eps_hatch(double phi) const {
	if (phi < 0)
		return 0;
	double f_phi = f(phi);
	return -params.eps_0 / ((f_phi + params.delta) * (f_phi + params.delta)) * f_hatch(phi);
}

void Model::iteration() { 
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		double phi_partial_x_x = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) / (dx * dx);
		double phi_partial_t = params.m * (0.5 * eps_hatch(phi[i]) * params.Phi_gradient *
			params.Phi_gradient + params.Gamma / (params.l * params.l) * f_hatch(phi[i]) +
			0.5 * params.Gamma * phi_partial_x_x);
		phi_next[i] = phi[i] + dt * phi_partial_t;
	}
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi[i] = phi_next[i];
	}
}

void Model::printValues(std::ostream& out) const {
	for (size_t i = 0; i <= params.x_grid; i += params.x_skip) {
		out << phi[i];
		if (i + params.x_skip <= params.x_grid)
			out << ";";
	}
	out << "\n";
}