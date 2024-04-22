#pragma once

#include "model/model.h"
#include "model/model_parameters.h"

#include <iostream>
#include <string>


class ModelToStationarySimple: public Model {
public:
	ModelToStationarySimple(const ModelParameters& params, NumericFunction&& phi_0,
		NumericFunction&& node, std::ostream& out);
	void setup(bool shall_print_phi, double gradient_threshold, double accending_threshold);

protected:
	bool shall_print_phi = true;
	double gradient_threshold = 0;
	double accending_threshold = std::numeric_limits<double>::infinity();
	double gradient_norm = std::numeric_limits<double>::infinity();
	double gradient_norm_previous;

protected:
	std::string getDescription() const override;
	bool shallPrint_phi() const override;
	void additionalOutput() const override;
	bool stopCondition() const override;
	void iterationAdditional() override;
};