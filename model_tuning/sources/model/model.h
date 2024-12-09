#pragma once

#include "model/model_parameters.h"
#include "display/progress_bar.h"

#include <iostream>
#include <vector>
#include <string>


class Model {
public:
	Model(ModelParameters&& params, std::ostream& out, const std::string& description =
		"A one-dimensional setting of the electrical breakdown problem",
		bool calculate_energy = true);
	void run();
	void write_phi(std::ostream&) const;

protected:
	ModelParameters params;
	std::ostream& out;
	std::string description;
	std::vector<double> phi;
	std::vector<double> phi_t;
	std::vector<double> phi_x_x;
	bool calculate_energy;
	std::vector<double> phi_x;
	std::vector<double> energy_density_electrical;
	std::vector<double> energy_density_border;
	std::vector<double> energy_density_inner;
	std::vector<double> energy_density_total;
	double energy_electrical;
	double energy_border;
	double energy_inner;
	ProgressBar progress_bar;

protected:
	void initialize();
	double f(double phi) const;
	double f_phi(double phi) const;
	double eps(size_t i) const;
	double eps_phi(size_t i) const;
	void iterationDerivatives();
	void iterationUpdate();
	void calculateEnergy();
	void printValues(std::ostream& out) const;
};