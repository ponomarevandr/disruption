#include "model.h"


Model::Model(const ModelParameters& params, NumericFunction&& phi_0, NumericFunction&& node,
		std::ostream& out, const std::string& details): params(params), phi_0(std::move(phi_0)),
		node(std::move(node)), out(out), details(details), progress_bar(params.t_grid) {
	dx = params.width / params.x_grid;
	dt = params.duration / params.t_grid;
}

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

double Model::rPowerDiff(size_t index, size_t order) const {
	return toPower(r_border[index + 1], order) - toPower(r_border[index], order);
}

void Model::initialize() {
	r_border.resize(params.x_grid + 1);
	s_border.resize(params.x_grid + 1);
	for (size_t i = 0; i <= params.x_grid; ++i) {
		r_border[i] = node(params, dx * i);
		s_border[i] = (params.r_power + 1) * toPower(r_border[i], params.r_power);
	}
	phi.resize(params.x_grid);
	dv.resize(params.x_grid);
	r.resize(params.x_grid);
	phi.push_back(1.0);  // Временный костыль для совпадения размеров вывода
	r.push_back(node(params, params.width));  // Временный костыль для совпадения размеров вывода
	for (size_t i = 0; i < params.x_grid; ++i) {
		dv[i] = rPowerDiff(i, params.r_power + 1);
		r[i] = 0.5 * (r_border[i] + r_border[i + 1]);
		phi[i] = phi_0(params, r[i]);
	}
	phi_grad_border.resize(params.x_grid + 1);
	phi_lapl.resize(params.x_grid);
	phi_grad_lapl_border.resize(params.x_grid + 1);
	flow_border.resize(params.x_grid + 1);
	phi_t.resize(params.x_grid);
	inter_coef_a_first_border.resize(params.x_grid + 1);
	inter_coef_a_second_border.resize(params.x_grid + 1);
	for (size_t i = 1; i < params.x_grid; ++i) {
		size_t order = params.r_power + 1;
		double determinant =
			rPowerDiff(i - 1, order + 1) * rPowerDiff(i, order) -
			rPowerDiff(i, order + 1) * rPowerDiff(i - 1, order);
		inter_coef_a_first_border[i] =
			(order + 1.0) / (params.r_power + 1.0) *
			rPowerDiff(params.x_grid + 1, i - 1) * rPowerDiff(order, i) / determinant;
		inter_coef_a_second_border[i] =
			(order + 1.0) / (params.r_power + 1.0) *
			-rPowerDiff(params.x_grid + 1, i) * rPowerDiff(order, i - 1) / determinant;
	}
	{
		size_t order = params.r_power + 3;
		double determinant =
			rPowerDiff(0, order + 1) * rPowerDiff(1, order) -
			rPowerDiff(1, order + 1) * rPowerDiff(0, order);
		inter_coef_a_first_higher =
			(order + 1.0) / (params.r_power + 1.0) *
			rPowerDiff(params.x_grid + 1, 0) * rPowerDiff(order, 1) / determinant;
		inter_coef_a_second_higher =
			(order + 1.0) / (params.r_power + 1.0) *
			-rPowerDiff(params.x_grid + 1, 1) * rPowerDiff(order, 0) / determinant;
		inter_coef_b_first_higher =
			order / (params.r_power + 1.0) *
			-rPowerDiff(params.x_grid + 1, 0) * rPowerDiff(order + 1, 1) / determinant;
		inter_coef_b_second_higher =
			order / (params.r_power + 1.0) *
			rPowerDiff(params.x_grid + 1, 1) * rPowerDiff(order + 1, 0) / determinant;
	}
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
	{
		double inter_a =
			phi[0] * inter_coef_a_first_higher +
			phi[1] * inter_coef_a_second_higher;
		double inter_b =
			phi[0] * inter_coef_b_first_higher +
			phi[1] * inter_coef_b_second_higher;
		phi_grad_border[1] =
			inter_a * 3.0 * r_border[1] * r_border[1] +
			inter_b * 2.0 * r_border[1];
	}
	for (size_t i = 2; i < params.x_grid; ++i) {
		double inter_a =
			phi[i - 1] * inter_coef_a_first_border[i] +
			phi[i] * inter_coef_a_second_border[i];
		phi_grad_border[i] = inter_a;
	}
	flow_border[0] = 0;
	for (size_t i = 1; i < params.x_grid; ++i) {
		flow_border[i] = 0.5 * phi_grad_border[i];
	}
	for (size_t i = 0; i + 1 < params.x_grid; ++i) {
		phi_t[i] = params.m * (
			0.5 * params.Phi_gradient * params.Phi_gradient * eps_phi(phi[i]) +
			params.Gamma / (params.l * params.l) * f_phi(phi[i]) +
			params.Gamma / dv[i] * (
				s_border[i + 1] * flow_border[i + 1] - s_border[i] * flow_border[i]
			)
		);
	}
	phi_t[params.x_grid - 1] = 0;
}

void Model::iterationUpdate() { 
	for (size_t i = 0; i < params.x_grid; ++i) {
		phi[i] += dt * phi_t[i];
	}
}

void Model::printValues(std::ostream& out, bool are_nodes) const {
	if (shallPrint_phi()) {
		for (size_t i = 0; i <= params.x_grid; i += params.x_skip) {
			out << (are_nodes ? r[i] : phi[i]);
			if (i + params.x_skip <= params.x_grid)
				out << ";";
		}
	}
	additionalOutput();
	out << "\n";
}

std::string Model::getDescription() const {
	return "A modeling of the breakdown problem by the finite volume method";
}

bool Model::shallPrint_phi() const {
	return true;
}

void Model::additionalOutput() const {}

bool Model::stopCondition() const {
	return false;
}

void Model::iterationAdditional() {}