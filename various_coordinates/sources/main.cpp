#include "model/model_parameters.h"
#include "model/model.h"

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
	Model model(params, phi_0, fout << std::scientific << std::setprecision(8));
	model.run();
	fout.close();
	
	return 0;
}