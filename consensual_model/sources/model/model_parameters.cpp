#include "model_parameters.h"


void ModelParameters::read(std::istream& in, bool read_phi_0) {
	environment.read(in);
	delta = environment.getVariableValue("delta");
	l = environment.getVariableValue("l");
	m = environment.getVariableValue("m");
	q = environment.getVariableValue("q");
	x_grid = environment.getVariableValue("x_grid");
	x_skip = environment.getVariableValue("x_skip");
	t_grid = environment.getVariableValue("t_grid");
	t_skip = environment.getVariableValue("t_skip");
	width = environment.getVariableValue("width");
	duration = environment.getVariableValue("duration");
	dt_max = environment.getVariableValue("dt_max");
	tol_phi = environment.getVariableValue("tol_phi");
	tol_energy = environment.getVariableValue("tol_energy");
	tol_stability = environment.getVariableValue("tol_stability");
	adaptation_type = environment.getVariableValue("adaptation_type");
	dx = width / x_grid;
	dt = duration / t_grid;
	eps_0.resize(x_grid + 1);
	Gamma.resize(x_grid + 1);
	phi_0.resize(x_grid + 1);
	for (size_t i = 0; i <= x_grid; ++i) {
		eps_0[i] = environment.getFunctionValue("eps_0", i * dx);
		Gamma[i] = environment.getFunctionValue("Gamma", i * dx);
		if (read_phi_0)
			phi_0[i] = environment.getFunctionValue("phi_0", i * dx);
	}
}

void ModelParameters::write(std::ostream& out) const {
	environment.write(out);
	out << "t_0 = " << t_0 << "\n";
}

void ModelParameters::read_phi_0(std::istream& in) {
	in.read(reinterpret_cast<char*>(&t_0), sizeof(double));
	in.read(reinterpret_cast<char*>(phi_0.data()), sizeof(double) * phi_0.size());
}