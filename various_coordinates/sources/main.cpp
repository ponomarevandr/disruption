#include "model/model_parameters.h"
#include "model/model.h"
#include "model_to_stationary/simple.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>


double phi_0(const ModelParameters& params, double x) {
	const double PI = 3.14159265358979324;
	const double DENT_WIDTH = 0.5;
	const double DENT_DEPTH = 0.05;
	x -= 0.5 * params.width;
	if (x <= -0.5 * DENT_WIDTH || 0.5 * DENT_WIDTH <= x)
		return 1.0;
	return 1.0 - 0.5 * DENT_DEPTH * (1.0 + std::cos(x / DENT_WIDTH * 2.0 * PI));
}

double phi_0_stationary(const ModelParameters& params, double x) {
	const double PI = 3.14159265358979324;
	if (x > params.l)
		return 1;
	return 0.5 - 0.5 * std::cos(x / params.l * PI);
}

double phi_0_stationary_lighter(const ModelParameters& params, double x) {
	const double PI = 3.14159265358979324;
	return 1.0 - std::exp(-15 * x);
}

double phi_0_stationary_lighter_2(const ModelParameters& params, double x) {
	const double PI = 3.14159265358979324;
	return 1.0 - std::exp(-70 * x);
}

double node_regular(const ModelParameters& params, double x) {
	return x;
}

double node_logarithmic(const ModelParameters& params, double x) {
	const double MIN_POWER = 3.0;
	return (std::exp((x / params.width - 1.0) * MIN_POWER) - std::exp(-MIN_POWER)) * params.width;
}

double node_logarithmic_2(const ModelParameters& params, double x) {
	const double MIN_POWER = 3.0;
	return (std::exp((x / params.width - 1.0) * MIN_POWER) - std::exp(-MIN_POWER)) * params.width;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Wrong number of arguments!\n";
		return 1;
	}

	std::ifstream fin(argv[1]);
	ModelParameters params;
	params.read(fin);
	fin.close();

	std::ofstream fout(argv[2]);
	ModelToStationarySimple model(params, phi_0_stationary, node_regular,
		fout << std::scientific << std::setprecision(8));
	model.setup(true, 0);
	model.run();
	fout.close();
	
	return 0;
}