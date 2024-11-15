#include "model/model_parameters.h"
#include "model/model.h"
#include "model/model_free_energy.h"

#include "evaluation/formula.h"
#include "evaluation/environment.h"

#include <iostream>
#include <fstream>
#include <iomanip>

#include <string>
#include <map>


int main(int argc, char* argv[]) {
	std::ifstream fin(argv[1]);
	Evaluation::Environment environment;
	environment.read(fin);
	fin.close();

	environment.write(std::cout);

	/*if (argc != 3) {
		std::cerr << "Wrong number of arguments!\n";
		return 1;
	}

	std::ifstream fin(argv[1]);
	ModelParameters params;
	params.read(fin);
	fin.close();

	std::ofstream fout(argv[2]);
	Model model(params, fout << std::scientific << std::setprecision(8));
	model.run();
	fout.close();*/
	
	return 0;
}