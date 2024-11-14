#pragma once


#include "model/model.h"
#include "model/model_parameters.h"

#include <iostream>
#include <vector>
#include <string>


class ModelFreeEnergy: public Model {
public:
	enum class Type {
		DENSITY_ELECTRICAL,
		DENSITY_INNER,
		DENSITY_BORDER,
		TOTAL
	};

public:
	ModelFreeEnergy(const ModelParameters& params, std::ostream& out, Type type);
	static std::string descriptionOfType(Type);
	void run();

protected:
	Type type;
	std::vector<double> phi_partial_x;
	std::vector<double> energy_density_electrical;
	std::vector<double> energy_density_inner;
	std::vector<double> energy_density_border;
	std::vector<double> energy_density_total;
	double energy_total;

protected:
	void initialize();
	double eps(double phi) const;
	void calculateEnergy();
	void printValues(std::ostream&);
};