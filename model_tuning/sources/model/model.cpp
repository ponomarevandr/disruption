#include "model.h"

#include <cmath>
#include <algorithm>


Model::Model(const ModelParameters& params, std::ostream& out, const std::string& description):
		params(params), out(out), description(description), progress_bar(params.t_grid) {
	dx = params.width / params.x_grid;
	dt = params.duration / params.t_grid;
}

void Model::run() {
	out << description << "\n";
	params.print(out);
	initialize();
	iterationDerivatives();
	printValues(out);
	progress_bar.update(0);
	for (size_t i = params.t_skip; i <= params.t_grid; i += params.t_skip) {
		for (size_t j = 0; j < params.t_skip; ++j) {
			iterationUpdate();
			iterationDerivatives();
		}
		printValues(out);
		progress_bar.update(i);
	}
}

double Model::phi_0(double x) const {
	const double PI = 3.14159265358979324;
	x -= 0.5 * params.width;
	if (x <= -0.5 * START_DENT_WIDTH || 0.5 * START_DENT_WIDTH <= x)
		return 1.0;
	return 1.0 - 0.5 * START_DENT_DEPTH * (1.0 + std::cos(x / START_DENT_WIDTH * 2.0 * PI));
}

void Model::initialize() {
	phi.resize(params.x_grid + 1);
	for (size_t i = 0; i <= params.x_grid; ++i) {
		phi[i] = phi_0(i * dx);
	}
	phi_t.resize(params.x_grid + 1);
	phi_x_x.resize(params.x_grid + 1);
}

double Model::f(double phi) const {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double Model::f_phi(double phi) const {
	return 12.0 * phi * phi * (1.0 - phi);
}

double Model::eps_phi(double phi) const {
	double f_value = f(phi);
	return -params.eps_0 / ((f_value + params.delta) * (f_value + params.delta)) * f_phi(phi);
}

void Model::iterationDerivatives() { 
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi_x_x[i] = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) / (dx * dx);
		phi_t[i] = params.m * (
			0.5 * eps_phi(phi[i]) * params.Phi_gradient * params.Phi_gradient +
			params.Gamma / (params.l * params.l) * f_phi(phi[i]) +
			0.5 * params.Gamma * phi_x_x[i]
		);
	}
}

void Model::iterationUpdate() {
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi[i] += dt * phi_t[i];
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