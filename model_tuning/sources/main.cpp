#include "model/model_parameters.h"
#include "model/model.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>


int main(int argc, char* argv[]) {
	std::vector<std::string> args(argv + 1, argv + argc);
	std::string filename_in;
	std::string filename_out;
	std::string filename_phi_in;
	std::string filename_phi_out;
	for (size_t i = 0; i < args.size(); ++i) {
		if (args[i] == "-i") {
			filename_phi_in = args[i + 1];
			++i;
			continue;
		}
		if (args[i] == "-o") {
			filename_phi_out = args[i + 1];
			++i;
			continue;
		}
		if (filename_in.empty()) {
			filename_in = args[i];
			continue;
		}
		if (filename_out.empty()) {
			filename_out = args[i];
			continue;
		}
		std::cerr << "Wrong number of parameters!\n";
		return 1;
	}
	if (filename_out.empty()) {
		std::cerr << "Wrong number of parameters!\n";
		return 1;
	}

	std::ifstream fin(filename_in);
	ModelParameters params;
	params.read(fin, filename_phi_in.empty());
	fin.close();

	if (!filename_phi_in.empty()) {
		std::ifstream fin_phi(filename_phi_in, std::ios::binary);
		params.read_phi_0(fin_phi);
		fin_phi.close();
	}

	std::ofstream fout(argv[2]);
	Model model(std::move(params), fout << std::scientific << std::setprecision(8));
	model.run();
	fout.close();

	if (!filename_phi_out.empty()) {
		std::ofstream fout_phi(filename_phi_out, std::ios::binary);
		model.write_phi(fout_phi);
		fout_phi.close();
	}
	
	return 0;
}