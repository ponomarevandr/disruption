#include "model_parameters.h"

#include <map>


void ModelParameters::read(std::istream& in) {
	environment.read(in);
	delta = environment.getVariableValue("delta");
	l = environment.getVariableValue("l");
	m = environment.getVariableValue("m");
	Phi_gradient = environment.getVariableValue("Phi_gradient");
	x_grid = environment.getVariableValue("x_grid");
	x_skip = environment.getVariableValue("x_skip");
	t_grid = environment.getVariableValue("t_grid");
	t_skip = environment.getVariableValue("t_skip");
	width = environment.getVariableValue("width");
	duration = environment.getVariableValue("duration");
	dx = width / x_grid;
	dt = duration / t_grid;
	initializeArrays();
}

void ModelParameters::write(std::ostream& out) const {
	environment.write(out);
}

void ModelParameters::initializeArrays() {
	eps_0.resize(x_grid + 1);
	Gamma.resize(x_grid + 1);
	phi_0.resize(x_grid + 1);
	for (size_t i = 0; i <= x_grid; ++i) {
		eps_0[i] = environment.getFunctionValue("eps_0", i * dx);
		Gamma[i] = environment.getFunctionValue("Gamma", i * dx);
		phi_0[i] = environment.getFunctionValue("phi_0", i * dx);
	}
}