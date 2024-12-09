#include "model.h"

#include <cmath>
#include <algorithm>


Model::Model(ModelParameters&& params, std::ostream& out, const std::string& description,
	bool calculate_energy): params(std::move(params)), out(out), description(description),
	calculate_energy(calculate_energy), progress_bar(params.t_grid) {}

void Model::run() {
	out << description << "\n\n";
	params.write(out);
	out << "\n";
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

void Model::write_phi(std::ostream& out) const {
	double t_end = params.t_0 + params.duration;
	out.write(reinterpret_cast<const char*>(&t_end), sizeof(double));
	out.write(reinterpret_cast<const char*>(phi.data()), sizeof(double) * phi.size());
}

void Model::initialize() {
	phi = params.phi_0;
	phi_t.resize(params.x_grid + 1);
	phi_x_x.resize(params.x_grid + 1);
	phi_x.resize(params.x_grid + 1);
	energy_density_electrical.assign(params.x_grid + 1, 0);
	energy_density_border.assign(params.x_grid + 1, 0);
	energy_density_inner.assign(params.x_grid + 1, 0);
	energy_electrical = energy_border = energy_inner = 0;
}

double Model::f(double phi) const {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double Model::f_phi(double phi) const {
	return 12.0 * phi * phi * (1.0 - phi);
}

double Model::eps(size_t i) const {
	return params.eps_0[i] / (f(phi[i]) + params.delta);
}

double Model::eps_phi(size_t i) const {
	double f_value = f(phi[i]);
	return -params.eps_0[i] / ((f_value + params.delta) * (f_value + params.delta)) * f_phi(phi[i]);
}

void Model::iterationDerivatives() { 
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi_x_x[i] = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) / (params.dx * params.dx);
		phi_t[i] = params.m * (
			0.5 * eps_phi(i) * params.Phi_gradient * params.Phi_gradient +
			params.Gamma[i] / (params.l * params.l) * f_phi(phi[i]) +
			0.5 * params.Gamma[i] * phi_x_x[i]
		);
	}
	if (calculate_energy)
		calculateEnergy();
}

void Model::iterationUpdate() {
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi[i] += params.dt * phi_t[i];
	}
}

void Model::calculateEnergy() {
	phi_x[0] = (phi[1] - phi[0]) / params.dx;
	phi_x[params.x_grid] = (phi[params.x_grid] - phi[params.x_grid - 1]) / params.dx;
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi_x[i] = 0.5 * (phi[i + 1] - phi[i - 1]) / params.dx;
	}
	for (size_t i = 0; i <= params.x_grid; ++i) {
		energy_density_electrical[i] = -0.5 * eps(i) * params.Phi_gradient * params.Phi_gradient;
		energy_density_border[i] = 0.25 * params.Gamma[i] * (phi_x[i] * phi_x[i]);
		energy_density_inner[i] = params.Gamma[i] * (1.0 - f(phi[i])) / (params.l * params.l);
	}
	energy_electrical = energy_border = energy_inner = 0;
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		energy_electrical += params.dx * energy_density_electrical[i];
		energy_border += params.dx * energy_density_border[i];
		energy_inner += params.dx * energy_density_inner[i];
	}
	energy_electrical += 0.5 * params.dx * (
		energy_density_electrical[0] + energy_density_electrical[params.x_grid]
	);
	energy_border += 0.5 * params.dx * (
		energy_density_border[0] + energy_density_border[params.x_grid]
	);
	energy_inner += 0.5 * params.dx * (
		energy_density_inner[0] + energy_density_inner[params.x_grid]
	);
}

void Model::printValues(std::ostream& out) const {
	for (size_t i = 0; i <= params.x_grid; i += params.x_skip) {
		// Уловка для уменьшения объема файла
		if (phi[i] == 1.0) {
			out << "1";
		} else if (phi[i] == 0) {
			out << "0";
		} else {
			out << phi[i];
		}
		if (i + params.x_skip <= params.x_grid)
			out << ";";
	}
	out << ";" << energy_electrical << ";" << energy_border << ";" << energy_inner;
	out << "\n";
}