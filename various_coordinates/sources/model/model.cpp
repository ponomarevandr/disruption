#include "model.h"

#include <algorithm>


Model::Model(const ModelParameters& params, Function_phi_0&& phi_0, std::ostream& out,
		const std::string& details): params(params), phi_0(std::move(phi_0)), out(out),
		details(details) {
	dx = params.width / params.x_grid;
	dt = params.duration / params.t_grid;
}

void Model::run() {
	out << toSingleLine(getDescription()) << "\n";
	out << toSingleLine(details) << "\n\n";
	params.print(out);
	out << (shallPrint_phi() ? "+" : "-") << "\n\n";
	initialize();
	printValues(out);
	if (stopCondition())
		return;
	for (size_t i = 1; i <= params.t_grid; i += params.t_skip) {
		for (size_t j = 0; j < params.t_skip; ++j) {
			iteration();
		}
		printValues(out);
		if (stopCondition())
			break;
	}
}

std::string Model::toSingleLine(const std::string& string) {
	std::string result = string;
	for (char& symbol: result) {
		if (symbol == '\n')
			symbol = ' ';
	}
	return result;
}

void Model::initialize() {
	phi.resize(params.x_grid + 1);
	for (size_t i = 0; i <= params.x_grid; ++i) {
		phi[i] = phi_0(params, i * dx);
	}
	phi_next.resize(params.x_grid + 1);
}

double Model::f(double phi) const {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double Model::f_phi(double phi) const {
	return 12.0 * phi * phi * (1.0 - phi);
}

double Model::eps_phi(double phi) const {
	double f_at_phi = f(phi);
	return -params.eps_0 / ((f_at_phi + params.delta) * (f_at_phi + params.delta)) * f_phi(phi);
}

void Model::iteration() { 
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		double phi_x_x = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) / (dx * dx);
		double phi_t = params.m * (0.5 * eps_phi(phi[i]) * params.Phi_gradient *
			params.Phi_gradient + params.Gamma / (params.l * params.l) * f_phi(phi[i]) +
			0.5 * params.Gamma * phi_x_x);
		phi_next[i] = phi[i] + dt * phi_t;
	}
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi[i] = phi_next[i];
	}
}

void Model::printValues(std::ostream& out) const {
	if (shallPrint_phi()) {
		for (size_t i = 0; i <= params.x_grid; i += params.x_skip) {
		out << phi[i];
		if (i + params.x_skip <= params.x_grid)
			out << ";";
		}
	}
	additionalOutput();
	out << "\n";
}

std::string Model::getDescription() const {
	return "A modeling of the basic breakdown problem";
}

bool Model::shallPrint_phi() const {
	return true;
}

void Model::additionalOutput() const {}

bool Model::stopCondition() const {
	return false;
}