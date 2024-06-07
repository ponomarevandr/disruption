#include "model.h"

#include <cmath>


Model::Model(const ModelParameters& params, NumericFunction&& phi_0, NumericFunction&& node,
		std::ostream& out, const std::string& details): params(params), phi_0(std::move(phi_0)),
		node(std::move(node)), out(out), details(details), progress_bar(params.t_grid) {
	dx = params.width / params.x_grid;
	dt = params.duration / params.t_grid;
	switch (params.r_power) {
	case 0:
		inter_case = params.alpha == 0 ? InterCase::QUADRATIC : InterCase::CUBIC;
		break;
	case 1:
		if (params.alpha != 0) {
			inter_case = InterCase::LOGARITHMIC;
		} else if (params.beta != 0) {
			inter_case = InterCase::POWER_TWO_THIRDS;
		} else {
			std::cout << "Case forbidden!\nr_power == 1,\nalpha == 0,\nbeta == 0\n";
			exit(0);
		}
		break;
	case 2:
		if (params.alpha != 0) {
			std::cout << "Case forbidden!\nr_power == 2,\nalpha != 0\n";
			exit(0);
		} else if (params.beta != 0) {
			inter_case = InterCase::POWER_ONE_THIRD;
		} else {
			std::cout << "Case forbidden!\nr_power == 2,\nalpha == 0,\nbeta == 0\n";
			exit(0);
		}
		break;
	default:
		std::cout << "Unknown r_power!\nr_power == " << params.r_power << "\n";
		exit(0);
	}
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

double Model::getRPowerDiff(size_t index, size_t order) const {
	return toPower(r_border[index + 1], order) - toPower(r_border[index], order);
}

Model::InterCoefficients::InterCoefficients(double a_first, double a_second, double b_first,
	double b_second): a_first(a_first), a_second(a_second), b_first(b_first), b_second(b_second) {}

Model::InterCoefficients Model::getInterCoefficients(size_t index, double integral_a_first,
		double integral_a_second, double integral_b_first, double integral_b_second) const {
	double determinant =
		integral_a_first * integral_b_second - integral_b_first * integral_a_second;
	double a_first =
		getRPowerDiff(index - 1, params.r_power + 1) * integral_b_second /
		determinant;
	double a_second =
		-getRPowerDiff(index, params.r_power + 1) * integral_b_first /
		determinant;
	double b_first =
		-getRPowerDiff(index - 1, params.r_power + 1) * integral_a_second /
		determinant;
	double b_second =
		getRPowerDiff(index, params.r_power + 1) * integral_a_first /
		determinant;
	return InterCoefficients(a_first, a_second, b_first, b_second);
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
	phi.push_back(1.0);  // Костыль для совпадения размеров вывода
	r.push_back(node(params, params.width));  // Костыль для совпадения размеров вывода
	for (size_t i = 0; i < params.x_grid; ++i) {
		dv[i] = getRPowerDiff(i, params.r_power + 1);
		r[i] = 0.5 * (r_border[i] + r_border[i + 1]);
		phi[i] = phi_0(params, r[i]);
	}
	phi_grad_border.resize(params.x_grid + 1);
	phi_lapl.resize(params.x_grid);
	phi_lapl_grad_border.resize(params.x_grid + 1);
	flow_density_border.resize(params.x_grid + 1);
	flow_border.resize(params.x_grid + 1);
	phi_t.resize(params.x_grid);
	inter_coef_a_first_border.resize(params.x_grid + 1);
	inter_coef_a_second_border.resize(params.x_grid + 1);
	for (size_t i = 1; i < params.x_grid; ++i) {
		size_t power = params.r_power + 1;
		InterCoefficients inter_coefficients = getInterCoefficients(
			i,
			getRPowerDiff(i - 1, power + 1) * power / (power + 1),
			getRPowerDiff(i, power + 1) * power / (power + 1),
			getRPowerDiff(i - 1, power),
			getRPowerDiff(i, power)
		);
		inter_coef_a_first_border[i] = inter_coefficients.a_first;
		inter_coef_a_second_border[i] = inter_coefficients.a_second;
	}
	if (inter_case == InterCase::QUADRATIC || inter_case == InterCase::CUBIC) {
		size_t power = params.r_power + (inter_case == InterCase::QUADRATIC ? 2 : 3);
		inter_coefs_higher = getInterCoefficients(
			1,
			getRPowerDiff(0, power + 1) * (params.r_power + 1) / (power + 1),
			getRPowerDiff(1, power + 1) * (params.r_power + 1) / (power + 1),
			getRPowerDiff(0, power) * (params.r_power + 1) / power,
			getRPowerDiff(1, power) * (params.r_power + 1) / power
		);
	}
	if (inter_case == InterCase::POWER_TWO_THIRDS) {
		double integral_a_first = 2.0 / (8.0 / 3.0) * (
			std::pow(r_border[1], 8.0 / 3.0) - 
				std::pow(r_border[0], 8.0 / 3.0)
		);
		double integral_a_second = 2.0 / (8.0 / 3.0) * (
			std::pow(r_border[2], 8.0 / 3.0) - 
				std::pow(r_border[1], 8.0 / 3.0)
		);
		inter_coefs_higher = getInterCoefficients(
			1,
			integral_a_first,
			integral_a_second,
			getRPowerDiff(0, 3) * 2.0 / 3.0,
			getRPowerDiff(1, 3) * 2.0 / 3.0
		);
	}
	if (inter_case == InterCase::POWER_ONE_THIRD) {
		double integral_a_first = 3.0 / (10.0 / 3.0) * (
			std::pow(r_border[1], 10.0 / 3.0) - 
				std::pow(r_border[0], 10.0 / 3.0)
		);
		double integral_a_second = 3.0 / (10.0 / 3.0) * (
			std::pow(r_border[2], 10.0 / 3.0) - 
				std::pow(r_border[1], 10.0 / 3.0)
		);
		inter_coefs_higher = getInterCoefficients(
			1,
			integral_a_first,
			integral_a_second,
			getRPowerDiff(0, 4) * 3.0 / 4.0,
			getRPowerDiff(1, 4) * 3.0 / 4.0
		);
	}
	if (inter_case == InterCase::LOGARITHMIC) {
		double integral_a_first = 1.0 / 8.0 *
			std::pow(r_border[1], 4) * (4.0 * std::log(r_border[1]) - 5.0);
		double integral_a_second = 1.0 / 8.0 * (
			std::pow(r_border[2], 4) * (4.0 * std::log(r_border[2]) - 5.0) -
				std::pow(r_border[1], 4) * (4.0 * std::log(r_border[1]) - 5.0)
		);
		inter_coefs_higher = getInterCoefficients(
			1,
			integral_a_first,
			integral_a_second,
			getRPowerDiff(0, 4) * 2.0 / 4.0,
			getRPowerDiff(1, 4) * 2.0 / 4.0
		);
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
	inter_a_higher =
		phi[0] * inter_coefs_higher.a_first +
		phi[1] * inter_coefs_higher.a_second;
	inter_b_higher =
		phi[0] * inter_coefs_higher.b_first +
		phi[1] * inter_coefs_higher.b_second;
	if (inter_case == InterCase::QUADRATIC) {
		phi_grad_border[0] = inter_b_higher;
		phi_grad_border[1] = inter_a_higher * 2.0 * r_border[1] + inter_b_higher;
	}
	if (inter_case == InterCase::CUBIC) {
		phi_grad_border[0] = 0;
		phi_grad_border[1] =
			inter_a_higher * 3.0 * r_border[1] * r_border[1] +
			inter_b_higher * 2.0 * r_border[1];
	}
	if (inter_case == InterCase::POWER_TWO_THIRDS) {
		phi_grad_border[0] = 0;
		phi_grad_border[1] =
			2.0 / 3.0 * inter_a_higher / std::pow(r_border[1], 1.0 / 3.0) + inter_b_higher;
	}
	if (inter_case == InterCase::POWER_ONE_THIRD) {
		phi_grad_border[0] = 0;
		phi_grad_border[1] =
			1.0 / 3.0 * inter_a_higher / std::pow(r_border[1], 2.0 / 3.0) + inter_b_higher;
	}
	if (inter_case == InterCase::LOGARITHMIC) {
		phi_grad_border[0] = 0;
		phi_grad_border[1] =
			inter_a_higher * r_border[1] * (2.0 * std::log(r_border[1]) - 1.0) +
			2.0 * inter_b_higher * r_border[1];
	}
	
	for (size_t i = 2; i < params.x_grid; ++i) {
		double inter_a =
			phi[i - 1] * inter_coef_a_first_border[i] +
			phi[i] * inter_coef_a_second_border[i];
		phi_grad_border[i] = inter_a;
	}
	for (size_t i = 0; i < params.x_grid; ++i) {
		flow_density_border[i] = 0.5 * phi_grad_border[i];
	}

	if (params.beta != 0) {
		for (size_t i = 0; i < params.x_grid; ++i) {
			flow_density_border[i] +=
				params.beta * params.l * params.l *
				phi_grad_border[i] * phi_grad_border[i] * phi_grad_border[i];
		}
	}

	if (params.alpha != 0) {
		for (size_t i = 0; i + 1 < params.x_grid; ++i) {
			phi_lapl[i] = (
				s_border[i + 1] * phi_grad_border[i + 1] - s_border[i] * phi_grad_border[i]
			) / dv[i];
		}
		phi_lapl[params.x_grid - 1] = 0;
		phi_lapl_grad_border[0] = 0;
		if (inter_case == InterCase::CUBIC) {
			phi_lapl_grad_border[0] = 6 * inter_a_higher;
		}
		for (size_t i = 1; i < params.x_grid; ++i) {
			double inter_c =
				phi_lapl[i - 1] * inter_coef_a_first_border[i] +
				phi_lapl[i] * inter_coef_a_second_border[i];
			phi_lapl_grad_border[i] = inter_c;
		}
		for (size_t i = 0; i < params.x_grid; ++i) {
			flow_density_border[i] -=
				params.alpha * 0.25 * params.l * params.l * phi_lapl_grad_border[i];
		}
	}

	for (size_t i = 0; i < params.x_grid; ++i) {
		flow_border[i] = s_border[i] * flow_density_border[i];
	}
	if (inter_case == InterCase::POWER_TWO_THIRDS) {
		flow_border[0] += params.beta * params.l * params.l *
			16.0 / 27.0 * inter_a_higher * inter_a_higher * inter_a_higher;
	}
	if (inter_case == InterCase::POWER_ONE_THIRD) {
		flow_border[0] += params.beta * params.l * params.l *
			1.0 / 9.0 * inter_a_higher * inter_a_higher * inter_a_higher;
	}
	if (inter_case == InterCase::LOGARITHMIC) {
		flow_border[0] -= params.alpha * 0.25 * params.l * params.l * 8.0 * inter_a_higher;
	}

	for (size_t i = 0; i + 1 < params.x_grid; ++i) {
		phi_t[i] = params.m * (
			0.5 * params.Phi_gradient * params.Phi_gradient * eps_phi(phi[i]) +
			params.Gamma / (params.l * params.l) * f_phi(phi[i]) +
			params.Gamma / dv[i] * (
				flow_border[i + 1] - flow_border[i]
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
		for (size_t i = 0; i <= INTERPOLATION_OUTPUT_GRID; ++i) {
			double current_r = r[0] / INTERPOLATION_OUTPUT_GRID * i;
			double current_phi;
			if (inter_case == InterCase::QUADRATIC) {
				current_phi = inter_a_higher * current_r * current_r + inter_b_higher * current_r;
			}
			if (inter_case == InterCase::CUBIC) {
				current_phi =
					inter_a_higher * current_r * current_r * current_r +
					inter_b_higher * current_r * current_r;
			}
			if (inter_case == InterCase::POWER_TWO_THIRDS) {
				current_phi =
					inter_a_higher * std::pow(current_r, 2.0 / 3.0) + inter_b_higher * current_r;
			}
			if (inter_case == InterCase::POWER_ONE_THIRD) {
				current_phi =
					inter_a_higher * std::pow(current_r, 1.0 / 3.0) + inter_b_higher * current_r;
			}
			if (inter_case == InterCase::LOGARITHMIC) {
				current_phi = i == 0 ? 0 : (
					inter_a_higher * current_r * current_r * (std::log(current_r) - 1) +
					inter_b_higher * current_r * current_r
				);
			}
			out << ";" << current_phi;
		}
		out << ";" << inter_a_higher << ";" << inter_b_higher;
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