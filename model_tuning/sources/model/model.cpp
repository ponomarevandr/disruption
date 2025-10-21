#include "model.h"

#include <cmath>
#include <algorithm>


Model::Model(ModelParameters&& params, std::ostream& out, const std::string& description,
		bool calculate_energy): params(std::move(params)), out(out), description(description),
		calculate_energy(calculate_energy), progress_bar(params.t_grid) {
	time_step_manager = TimeStepManager(this->params.t_grid, this->params.dt);
	this->params.t_skip = 1ull << log2Floor(this->params.t_skip);
}

size_t Model::log2Floor(size_t value) {
	size_t result = 0;
	while (value > 1) {
		value >>= 1ull;
		++result;
	}
	return result;
}

void Model::run() {
	out << description << "\n\n";
	params.write(out);
	out << "\n";
	initialize();
	iterationDerivatives();
	printValues(out);
	progress_bar.update(0);
	for (size_t i = 1; i <= params.t_grid; i += time_step_manager.getScale()) {
		iterationUpdate();
		iterationDerivatives();
		if (i % params.t_skip == 0)
			printValues(out);
		progress_bar.update(i);
		if (i < params.t_grid)
			time_step_manager.nextStep(dt_adaptive);
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
	instability_value.assign(params.x_grid + 1, 0);
}

double Model::f(double phi) const {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double Model::f_phi(double phi) const {
	return 12.0 * phi * phi * (1.0 - phi);
}


#ifdef EPS_NEW

double Model::power(double x, uint32_t p) {
	double result = 1.0;
	for (uint32_t i = 0; i < p; ++i) {
		result *= x;
	}
	return result;
}

double Model::eps_u(double x) const {
	return
		params.delta * (x - 1.0) +
		1.0 +
		-(1.0 - params.delta) * power(1.0 - x, EPS_U_POWER);
}

double Model::eps_u_x(double x) const {
	return
		params.delta +
		(1.0 - params.delta) * EPS_U_POWER * power(1.0 - x, EPS_U_POWER - 1);
}

double Model::eps(size_t i) const {
	return params.eps_0[i] * (
		1.0 + 1.0 / params.delta +
		-1.0 / params.delta * eps_u(f(phi[i]))
	);
}

double Model::eps_phi(size_t i) const {
	return -params.eps_0[i] / params.delta * eps_u_x(f(phi[i])) * f_phi(phi[i]);
}

#else

double Model::eps(size_t i) const {
	return params.eps_0[i] / (f(phi[i]) + params.delta);
}

double Model::eps_phi(size_t i) const {
	double f_value = f(phi[i]);
	return
		-params.eps_0[i] / ((f_value + params.delta) * (f_value + params.delta)) * f_phi(phi[i]);
}

#endif


double Model::instabilityFunction(size_t i) const {
	double f_value = f(phi[i]);
	double phi_cubed = phi[i] * phi[i] * phi[i];
	double eps_phi_phi_majorant = params.eps_0[i] * 12.0 * phi[i] * (
		16.0 * phi_cubed +
		-30.0 * phi_cubed * phi[i] +
		15.0 * phi_cubed * phi[i] * phi[i] +
		3 * phi[i] * params.delta +
		2 * params.delta
	) / (
		(f_value + params.delta) * (f_value + params.delta) * (f_value + params.delta)
	);
	return 0.5 * params.m * params.Phi_gradient * params.Phi_gradient * eps_phi_phi_majorant;
}

void Model::iterationDerivatives() {
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi_x_x[i] = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) / (params.dx * params.dx);
		phi_t[i] = params.m * (
			0.5 * eps_phi(i) * params.Phi_gradient * params.Phi_gradient +
			params.Gamma[i] / (params.l * params.l) * f_phi(phi[i])

#ifndef NO_LAPLACIAN
			+ 0.5 * params.Gamma[i] * phi_x_x[i]
#endif

		);
		instability_value[i] = instabilityFunction(i);
	}
	if (calculate_energy)
		calculateEnergy();
	calculateTimeStep();
	++t_iterations;
}

void Model::iterationUpdate() {
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		phi[i] += (params.dt * time_step_manager.getScale()) * phi_t[i];
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
	energy_total = energy_electrical + energy_border + energy_inner;
	energy_total_t =
		(energy_total - energy_total_previous) / (params.dt * time_step_manager.getScale());
	energy_total_previous = energy_total;
}

double Model::timeStepBounded(double time_step) const {
	return std::max(params.dt, std::min(params.dt_max, time_step));
}

void Model::calculateTimeStep() {
	dt_adaptive_phi = timeStepBounded(params.tol_phi / normUniform(phi_t));
	dt_adaptive_energy = timeStepBounded(params.tol_energy / std::abs(energy_total_t));
	dt_adaptive_stability = timeStepBounded(params.tol_stability / normUniform(instability_value));
	switch (params.adaptation_type) {
	case 1:
		dt_adaptive = dt_adaptive_phi;
		break;
	case 2:
		dt_adaptive = dt_adaptive_energy;
		break;
	case 3:
		dt_adaptive = dt_adaptive_stability;
		break;
	default:
		dt_adaptive = params.dt;
		break;
	}
}

double Model::normUniform(const std::vector<double>& f) {
	double result = 0;
	for (double value : f) {
		result = std::max(result, std::abs(value));
	}
	return result;
}


void Model::printValues(std::ostream& out) const {
	out << std::max(time_step_manager.getLevel(), log2Floor(params.t_skip)) << ";";
	out << t_iterations << ";";
	t_iterations = 0;
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
	out << ";" << normUniform(phi_t);
	out << ";" << std::abs(energy_total_t);
	out << ";" << normUniform(instability_value);
	out << ";" << dt_adaptive_phi << ";" << dt_adaptive_energy << ";" << dt_adaptive_stability;
	out << "\n";
}