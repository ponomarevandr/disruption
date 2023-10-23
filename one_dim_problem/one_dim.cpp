#include <fstream>
#include <iomanip>
#include <algorithm>


const double eps_0 = 1e-3;
const double delta = 1e-6;
const double l = 1.0;
const double Gamma = 0.01;
const double m = 1.0;
const double Phi_coefficient = 1.0;

const double duration = 1.0;
const double width = 10;

constexpr size_t X_SIZE = 1000;
constexpr size_t T_SIZE = 100000;
constexpr size_t X_SKIP = 1;
constexpr size_t T_SKIP = 1000;

const char* output_filename = "phi_values.csv";
const size_t precision = 4;

double phi[X_SIZE];
double phi_next[X_SIZE];


void initialize() {
	for (size_t i = 0; i < X_SIZE; ++i) {
		phi[i] = 1.0;
	}
	phi[X_SIZE / 2] = 0.5;
}

double f(double phi) {
	double phi_cubed = phi * phi * phi;
	return 4 * phi_cubed - 3 * phi_cubed * phi;
}

double f_hatch(double phi) {
	return 12.0 * phi * phi * (1.0 - phi);
}

double eps_hatch(double phi) {
	double f_phi = f(phi);
	return eps_0 / ((f_phi + delta) * (f_phi + delta)) * f_hatch(phi);
}

void iteration() {
	double dx = width / X_SIZE;
	double dt = duration / T_SIZE; 
	for (size_t i = 1; i < X_SIZE - 1; ++i) {
		double phi_partial_x_x = (phi[i - 1] - 2.0 * phi[i] + phi[i + 1]) / (dx * dx);
		double phi_partial_t = m * (0.5 * eps_hatch(phi[i]) * Phi_coefficient * Phi_coefficient +
			+ Gamma / l * f_hatch(phi[i]) + 0.5 * Gamma * phi_partial_x_x);
		phi_next[i] = phi[i] + dt * phi_partial_t;
	}
	for (size_t i = 1; i < X_SIZE - 1; ++i) {
		phi[i] = phi_next[i];
		phi[i] = std::min(phi[i], 1.0);
		phi[i] = std::max(phi[i], 0.0);
	}
}

void printValues(std::ostream& fout) {
	for (size_t i = 0; i < X_SIZE; i += X_SKIP) {
		fout << std::scientific << std::setprecision(precision) << phi[i];
		if (i + X_SKIP < X_SIZE)
			fout << ";";
	}
	fout << "\n";
}


int main() {
	std::ofstream fout(output_filename);
	
	initialize();
	printValues(fout);
	for (size_t i = 0; i < T_SIZE; i += T_SKIP) {
		for (size_t j = 0; j < T_SKIP; ++j) {
			iteration();
		}
		printValues(fout);
	}

	fout.close();
	return 0;
}