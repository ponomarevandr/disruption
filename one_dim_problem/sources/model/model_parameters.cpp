#include "model_parameters.h"


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