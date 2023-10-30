#include "model.h"

#include <cmath>
#include <algorithm>


Model::Model(const ModelParameters& params, std::ostream& out, const std::string& description):
		params(params), out(out), description(description) {
	t_size = static_cast<size_t>(std::round(params.duration / params.dt)) + 1;
	x_size = static_cast<size_t>(std::round(params.width / params.dx)) + 1;
}

void Model::run() {
	out << description << "\n";
	params.print(out);
	initialize();
	printValues(out);
	for (size_t i = 0; i < t_size; i += params.t_skip) {
		for (size_t j = 0; j < params.t_skip; ++j) {
			iteration();
		}
		printValues(out);
	}
}

void Model::initialize() {
	phi.assign(x_size, 1.0);
	phi[x_size / 2] = 0.9;
	phi_next.resize(x_size);
}

double Model::f(double phi) const {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double Model::f_hatch(double phi) const {
	return 12.0 * phi * phi * (1.0 - phi);
}

double Model::eps_hatch(double phi) const {
	double f_phi = f(phi);
	return -params.eps_0 / ((f_phi + params.delta) * (f_phi + params.delta)) * f_hatch(phi);
}

void Model::iteration() { 
	for (size_t i = 1; i + 1 < x_size; ++i) {
		double phi_partial_x_x = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) /
			(params.dx * params.dx);
		double phi_partial_t = params.m * (0.5 * eps_hatch(phi[i]) * params.Phi_coefficient *
			params.Phi_coefficient + params.Gamma / params.l * f_hatch(phi[i]) +
			0.5 * params.Gamma * phi_partial_x_x);
		phi_next[i] = phi[i] + params.dt * phi_partial_t;
	}
	for (size_t i = 1; i + 1 < x_size; ++i) {
		phi[i] = phi_next[i];
		//phi[i] = std::min(phi[i], 1.0);
		//phi[i] = std::max(phi[i], 0.0);
	}
}

void Model::printValues(std::ostream& out) const {
	for (size_t i = 0; i < x_size; i += params.x_skip) {
		out << phi[i];
		if (i + params.x_skip < x_size)
			out << ";";
	}
	out << "\n";
}