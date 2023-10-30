#include "model/model_parameters.h"
#include "model/model.h"

#include <fstream>
#include <iomanip>


int main() {
	std::ofstream fout("phi_values.csv");
	ModelParameters params;
	params.setPhysical(1e-3, 1e-6, 1.0, 1.0, 1.0, 60.0);
	params.setComputational(0.01, 1, 1e-5, 1000);
	params.setSizes(10.0, 1.0);
	Model model(params, fout << std::scientific << std::setprecision(8));
	model.run();
	fout.close();
	return 0;
}