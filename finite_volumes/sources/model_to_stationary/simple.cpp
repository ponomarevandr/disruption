#include "simple.h"

#include <algorithm>


ModelToStationarySimple::ModelToStationarySimple(const ModelParameters& params,
	NumericFunction&& phi_0, NumericFunction&& node, std::ostream& out):
	Model(params, std::move(phi_0), std::move(node), out) {}

void ModelToStationarySimple::setup(bool shall_print_phi, double gradient_threshold,
		double accending_threshold) {
	this->shall_print_phi = shall_print_phi;
	this->gradient_threshold = gradient_threshold;
	this->accending_threshold = accending_threshold;
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
	out << gradient_norm;
}

bool ModelToStationarySimple::stopCondition() const {
	if (gradient_norm <= gradient_threshold)
		return true;
	return gradient_norm > accending_threshold && gradient_norm_previous < gradient_norm;
}

void ModelToStationarySimple::iterationAdditional() {
	gradient_norm_previous = gradient_norm;
	gradient_norm = 0;
	for (size_t i = 0; i <= params.x_grid; ++i) {
		gradient_norm = std::max(gradient_norm, std::abs(phi_t[i]) / params.m);
	}
	gradient_norm /= params.m;
}