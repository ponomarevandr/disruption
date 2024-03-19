#pragma once

#include "model/model_parameters.h"

#include <iostream>
#include <vector>
#include <string>


class ScaledModel {
public:
	ScaledModel(const ModelParameters& params, std::ostream& out, const std::string& description =
		"A model with a variable substitution for the cylindical case");
	void run();

private:
	ModelParameters params;
	std::ostream& out;
	std::string description;
	double dx;
	double dt;
	std::vector<double> phi;
	std::vector<double> phi_next;

private:
	double phi_0(double x) const;
	void initialize();
	double f(double phi) const;
	double f_hatch(double phi) const;
	void iteration();
	void printValues(std::ostream& out) const;
};