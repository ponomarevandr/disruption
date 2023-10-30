#include "model/model_parameters.h"
#include "model/model.h"

#include <iostream>
#include <fstream>
#include <iomanip>


int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Wrong number of arguments!\n";
		return 1;
	}

	std::ifstream fin(argv[1]);
	ModelParameters params;
	params.read(fin);
	fin.close();
	//params.setPhysical(1e-3, 1e-6, 1.0, 1.0, 1.0, 60.0);
	//params.setComputational(1e-5, 1000, 0.01, 1);
	//params.setSizes(1.0, 10.0);

	std::ofstream fout(argv[2]);
	Model model(params, fout << std::scientific << std::setprecision(8));
	model.run();
	fout.close();
	return 0;
}