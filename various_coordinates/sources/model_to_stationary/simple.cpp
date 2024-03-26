#include "simple.h"

#include <algorithm>


ModelToStationarySimple::ModelToStationarySimple(const ModelParameters& params,
	NumericFunction&& phi_0, NumericFunction&& node, std::ostream& out):
	Model(params, std::move(phi_0), std::move(node), out) {}

void ModelToStationarySimple::setup(bool shall_print_phi, double gradient_threshold) {
	this->shall_print_phi = shall_print_phi;
	this->gradient_threshold = gradient_threshold;
}

double ModelToStationarySimple::getGradientNorm() const {
	double result = 0;
	for (size_t i = 1; i + 1 <= params.x_grid; ++i) {
		result = std::max(result, std::abs(phi_t[i]));
	}
	result /= params.m;
	return result;
}

std::string ModelToStationarySimple::getDescription() const {
	return "A search for the stationary solution";
}

bool ModelToStationarySimple::shallPrint_phi() const {
	return shall_print_phi;
}

void ModelToStationarySimple::additionalOutput() const {
	if (shall_print_phi)
		out << ";";
	out << getGradientNorm();
}

bool ModelToStationarySimple::stopCondition() const {
	return getGradientNorm() <= gradient_threshold;
}