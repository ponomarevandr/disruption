#include "model_parameters.h"

#include <map>


void ModelParameters::setPhysical(double eps_0, double delta, double l, double Gamma, double m,
		double Phi_coefficient) {
	this->eps_0 = eps_0;
	this->delta = delta;
	this->l = l;
	this->Gamma = Gamma;
	this->m = m;
	this->Phi_gradient = Phi_gradient;
}

void ModelParameters::setComputational(size_t x_grid, size_t x_skip, size_t t_grid, size_t t_skip) {
	this->x_grid = x_grid;
	this->x_skip = x_skip;
	this->t_grid = t_grid;
	this->t_skip = t_skip;
}

void ModelParameters::setSizes(double width, double duration) {
	this->width = width;
	this->duration = duration;
}

void ModelParameters::print(std::ostream& out) const {
	out << PARAMETERS_NUMBER << " parameters:\n";
	out << "eps_0 = " << eps_0 << "\n";
	out << "delta = " << delta << "\n";
	out << "l = " << l << "\n";
	out << "Gamma = " << Gamma << "\n";
	out << "m = " << m << "\n";
	out << "Phi_gradient = " << Phi_gradient << "\n";
	out << "x_grid = " << x_grid << "\n";
	out << "x_skip = " << x_skip << "\n";
	out << "t_grid = " << t_grid << "\n";
	out << "t_skip = " << t_skip << "\n";
	out << "width = " << width << "\n";
	out << "duration = " << duration << "\n";
}

void ModelParameters::read(std::istream& in) {
	std::map<std::string, std::string> values;
	std::string line;
	while (std::getline(in, line)) {
		std::vector<std::string> splitted = split(line);
		values[splitted[0]] = splitted[2];
	}
	eps_0 = fromString<double>(values.at("eps_0"));
	delta = fromString<double>(values.at("delta"));
	l = fromString<double>(values.at("l"));
	Gamma = fromString<double>(values.at("Gamma"));
	m = fromString<double>(values.at("m"));
	Phi_gradient = fromString<double>(values.at("Phi_gradient"));
	x_grid = fromString<size_t>(values.at("x_grid"));
	x_skip = fromString<size_t>(values.at("x_skip"));
	t_grid = fromString<size_t>(values.at("t_grid"));
	t_skip = fromString<size_t>(values.at("t_skip"));
	width = fromString<double>(values.at("width"));
	duration = fromString<double>(values.at("duration"));
}

std::vector<std::string> ModelParameters::split(const std::string& text) {
	std::vector<std::string> result(1);
	for (char c : text) {
		if (std::isspace(c)) {
			if (!result.back().empty())
				result.emplace_back();
			continue;
		}
		result.back().push_back(c);
	}
	if (result.back().empty())
		result.pop_back();
	return result;
}