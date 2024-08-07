#include "model_free_energy.h"


ModelFreeEnergy::ModelFreeEnergy(const ModelParameters& params, std::ostream& out, Type type):
	type(type), Model(params, out, descriptionOfType(type)) {}

std::string ModelFreeEnergy::descriptionOfType(Type type) {
	switch (type) {
	case Type::DENSITY_ELECTRICAL:
		return "A model with electrical free energy density in the output";
	case Type::DENSITY_INNER:
		return "A model with inner free energy density in the output";
	case Type::DENSITY_BORDER:
		return "A model with border free energy density in the output";
	case Type::TOTAL:
		return "A model with total free energy in the output";
	}
	return "Unknown";
}

void ModelFreeEnergy::run() {
	out << description << "\n";
	params.print(out);
	initialize();
	calculateEnergy();
	printValues(out);
	for (size_t i = 1; i <= params.t_grid; i += params.t_skip) {
		for (size_t j = 0; j < params.t_skip; ++j) {
			iteration();
			calculateEnergy();
		}
		printValues(out);
	}
}

void ModelFreeEnergy::initialize() {
	Model::initialize();
	phi_partial_x.resize(params.x_grid + 1);
	energy_density_electrical.resize(params.x_grid + 1);
	energy_density_inner.resize(params.x_grid + 1);
	energy_density_border.resize(params.x_grid + 1);
	energy_density_total.resize(params.x_grid + 1);
}

double ModelFreeEnergy::eps(double phi) const {
	return params.eps_0 / (f(phi) + params.delta);
}

void ModelFreeEnergy::calculateEnergy() {
	for (size_t i = 0; i <= params.x_grid; ++i) {
		energy_density_electrical[i] =
			-0.5 * params.Phi_gradient * params.Phi_gradient * eps(phi[i]);
		energy_density_inner[i] = params.Gamma / (params.l * params.l) * (1.0 - f(phi[i]));
	}
	phi_partial_x[0] = (phi[1] - phi[0]) / dx;
	phi_partial_x[params.x_grid] = (phi[params.x_grid] - phi[params.x_grid - 1]) / dx;
	for (size_t i = 1; i < params.x_grid; ++i) {
		phi_partial_x[i] = 0.5 * (phi[i + 1] - phi[i - 1]) / dx;
	}
	for (size_t i = 0; i <= params.x_grid; ++i) {
		energy_density_border[i] = 0.25 * params.Gamma * phi_partial_x[i] * phi_partial_x[i];
	}
	for (size_t i = 0; i <= params.x_grid; ++i) {
		energy_density_total[i] =
			energy_density_electrical[i] +
			energy_density_inner[i] +
			energy_density_border[i];
	}
	energy_total = 0.5 * dx * (energy_density_total[0] + energy_density_total[params.x_grid]);
	for (size_t i = 1; i < params.x_grid; ++i) {
		energy_total += dx * energy_density_total[i];
	}
}

void ModelFreeEnergy::printValues(std::ostream& out) {
	if (type == Type::TOTAL) {
		out << energy_total << "\n";
		return;
	}
	for (size_t i = 0; i <= params.x_grid; i += params.x_skip) {
		switch (type) {
		case Type::DENSITY_ELECTRICAL:
			out << energy_density_electrical[i];
			break;
		case Type::DENSITY_INNER:
			out << energy_density_inner[i];
			break;
		case Type::DENSITY_BORDER:
			out << energy_density_border[i];
			break;
		}
		if (i + params.x_skip <= params.x_grid)
			out << ";";
	}
	out << "\n";
}