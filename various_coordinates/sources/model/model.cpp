#include "model.h"


Model::Model(const ModelParameters& params, BorderConditions&& border, NumericFunction&& node,
		std::ostream& out, const std::string& details): params(params), border(std::move(border)),
		node(std::move(node)), out(out), details(details), progress_bar(params.t_grid) {
	dx = params.width / params.x_grid;
	dt = params.duration / params.t_grid;
}

Model::BorderConditions::BorderConditions(NumericFunction&& phi_0, double phi_left,
	double phi_right, double phi_x_left, double phi_x_right): phi_0(std::move(phi_0)),
	phi_left(phi_left), phi_right(phi_right), phi_x_left(phi_x_left), phi_x_right(phi_x_right) {}

void Model::run() {
	out << toSingleLine(getDescription()) << "\n";
	out << toSingleLine(details) << "\n\n";
	params.print(out);
	out << (shallPrint_phi() ? "+" : "-") << "\n\n";
	initialize();
	iterationDerivatives();
	iterationAdditional();
	printValues(out, true);
	printValues(out);
	progress_bar.update(0);
	if (stopCondition())
		return;
	for (size_t i = 0; i < params.t_grid; i += params.t_skip) {
		for (size_t j = 0; j < params.t_skip; ++j) {
			iterationUpdate();
			iterationDerivatives();
			iterationAdditional();
		}
		printValues(out);
		progress_bar.update(i + params.t_skip);
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

double Model::toPower(double value, size_t power) {
	double result = 1.0;
	for (size_t i = 0; i < power; ++i) {
		result *= value;
	}
	return result;
}

void Model::initialize() {
	r.resize(params.x_grid + 2);
	phi.resize(params.x_grid + 2, 0);
	for (size_t i = 1; i <= params.x_grid; ++i) {
		r[i] = node(params, (i - 0.5) * dx);
		phi[i] = border.phi_0(params, r[i]);
	}
	r[0] = -r[1];
	r[params.x_grid + 1] = 2.0 * params.width - r[params.x_grid];

	r_mid.resize(params.x_grid + 3);
	dr_mid.resize(params.x_grid + 3);
	for (size_t i = 1; i <= params.x_grid + 1; ++i) {
		r_mid[i] = node(params, (i - 1) * dx);
		dr_mid[i] = r[i] - r[i - 1];
	}
	r_mid[0] = -r_mid[2];
	r_mid[params.x_grid + 2] = 2.0 * params.width - r_mid[params.x_grid];

	dr.resize(params.x_grid + 2);
	r_coef.resize(params.x_grid + 2);
	r[0] = r[1];
	for (size_t i = 0; i <= params.x_grid + 1; ++i) {
		dr[i] = r_mid[i + 1] - r_mid[i];
		r_coef[i] = toPower(r[i], params.r_power);
	}

	r_coef_mid.resize(params.x_grid + 3);
	r_mid[0] = r_mid[2];
	for (size_t i = 0; i <= params.x_grid + 2; ++i) {
		r_coef_mid[i] = toPower(r_mid[i], params.r_power);
	}

	phi_r_mid.resize(params.x_grid + 3, 0);
	phi_r_cubed_mid.resize(params.x_grid + 3, 0);
	phi_x_x_r_mid.resize(params.x_grid + 3, 0);
	phi_x_x.resize(params.x_grid + 2, 0);
	phi_x_cubed_x.resize(params.x_grid + 2, 0);
	phi_x_x_x_x.resize(params.x_grid + 2, 0);
	phi_t.resize(params.x_grid + 2, 0);
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

void Model::iterationDerivatives() {
	phi[0] = 2.0 * border.phi_left - phi[1];
	phi[params.x_grid + 1] = 2.0 * border.phi_right - phi[params.x_grid];
	phi_r_mid[0] = border.phi_x_left;
	phi_r_mid[params.x_grid + 2] = border.phi_x_right;

	for (size_t i = 1; i <= params.x_grid + 1; ++i) {
		phi_r_mid[i] = (phi[i] - phi[i - 1]) / dr_mid[i];
	}
	for (size_t i = 0; i <= params.x_grid + 1; ++i) {
		phi_x_x[i] = 1.0 / r_coef[i] * (r_coef_mid[i + 1] * phi_r_mid[i + 1] -
			r_coef_mid[i] * phi_r_mid[i]) / dr[i];
	}

	if (params.beta != 0) {
		for (size_t i = 0; i <= params.x_grid + 2; ++i) {
			phi_r_cubed_mid[i] = phi_r_mid[i] * phi_r_mid[i] * phi_r_mid[i];
		}
		for (size_t i = 0; i <= params.x_grid + 1; ++i) {
			phi_x_cubed_x[i] = 1.0 / r_coef[i] * (r_coef_mid[i + 1] * phi_r_cubed_mid[i + 1] - 
				r_coef_mid[i] * phi_r_cubed_mid[i]) / dr[i];
		}
	}

	if (params.alpha != 0) {
		for (size_t i = 1; i <= params.x_grid + 1; ++i) {
			phi_x_x_r_mid[i] = (phi_x_x[i] - phi_x_x[i - 1]) / dr_mid[i];
		}
		for (size_t i = 1; i <= params.x_grid; ++i) {
			phi_x_x_x_x[i] = 1.0 / r_coef[i] * (r_coef_mid[i + 1] * phi_x_x_r_mid[i + 1] -
				r_coef_mid[i] * phi_x_x_r_mid[i]) / dr[i];
		}
	}

	for (size_t i = 1; i <= params.x_grid; ++i) {
		phi_t[i] = params.m * (
			0.5 * eps_phi(phi[i]) * params.Phi_gradient * params.Phi_gradient +
			params.Gamma / (params.l * params.l) * f_phi(phi[i]) +
			0.5 * params.Gamma * phi_x_x[i] +
			params.beta * params.Gamma * params.l * params.l * phi_x_cubed_x[i] -
			0.25 * params.alpha * params.Gamma * params.l * params.l * phi_x_x_x_x[i]
		);
	}
}

void Model::iterationUpdate() { 
	for (size_t i = 1; i <= params.x_grid; ++i) {
		phi[i] += dt * phi_t[i];
	}
}

void Model::printValues(std::ostream& out, bool are_nodes) const {
	if (shallPrint_phi()) {
		for (size_t i = 0; i <= params.x_grid; i += params.x_skip) {
			out << (are_nodes ? 0.5 * (r[i + 1] + r[i]) : 0.5 * (phi[i + 1] + phi[i]));
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

void Model::iterationAdditional() {}