#pragma once

#include "model/model_parameters.h"

#include <iostream>
#include <vector>
#include <string>


class Model {
public:
	Model(const ModelParameters& params, std::ostream& out, const std::string& description =
		"A one-dimensional setting of the electrical disruption problem");
	void run();

private:
	ModelParameters params;
	std::ostream& out;
	std::string description;
	size_t x_size;
	size_t t_size;
	std::vector<double> phi;
	std::vector<double> phi_next;

private:
	void initialize();
	double f(double phi) const;
	double f_hatch(double phi) const;
	double eps_hatch(double phi) const;
	void iteration();
	void printValues(std::ostream& out) const;
};