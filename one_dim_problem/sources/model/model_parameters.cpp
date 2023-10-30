#include "model_parameters.h"

#include <map>


void ModelParameters::setPhysical(double eps_0, double delta, double l, double Gamma, double m,
		double Phi_coefficient) {
	this->eps_0 = eps_0;
	this->delta = delta;
	this->l = l;
	this->Gamma = Gamma;
	this->m = m;
	this->Phi_coefficient = Phi_coefficient;
}

void ModelParameters::setComputational(double dt, size_t t_skip, double dx, size_t x_skip) {
	this->dt = dt;
	this->t_skip = t_skip;
	this->dx = dx;
	this->x_skip = x_skip;
}

void ModelParameters::setSizes(double duration, double width) {
	this->duration = duration;
	this->width = width;
}

void ModelParameters::print(std::ostream& out) const {
	out << "eps_0 = " << eps_0 << "\n";
	out << "delta = " << delta << "\n";
	out << "l = " << l << "\n";
	out << "Gamma = " << Gamma << "\n";
	out << "m = " << m << "\n";
	out << "Phi_coefficient = " << Phi_coefficient << "\n";
	out << "dt = " << dt << "\n";
	out << "t_skip = " << t_skip << "\n";
	out << "dx = " << dx << "\n";
	out << "x_skip = " << x_skip << "\n";
	out << "duration = " << duration << "\n";
	out << "width = " << width << "\n";
}

void ModelParameters::read(std::istream& in) {
	std::map<std::string, std::string> values;
	std::string line;
	while (std::getline(in, line)) {
		std::vector<std::string> splitted = split(line);
		values[splitted[0]] = splitted[2];
	}
	eps_0 = fromString<double>(values["eps_0"]);
	delta = fromString<double>(values["delta"]);
	l = fromString<double>(values["l"]);
	Gamma = fromString<double>(values["Gamma"]);
	m = fromString<double>(values["m"]);
	Phi_coefficient = fromString<double>(values["Phi_coefficient"]);
	dt = fromString<double>(values["dt"]);
	t_skip = fromString<size_t>(values["t_skip"]);
	dx = fromString<double>(values["dx"]);
	x_skip = fromString<size_t>(values["x_skip"]);
	duration = fromString<double>(values["duration"]);
	width = fromString<double>(values["width"]);
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
	return result;
}