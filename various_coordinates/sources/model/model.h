#pragma once

#include "model/model_parameters.h"
#include "display/progress_bar.h"

#include <iostream>
#include <vector>
#include <string>
#include <functional>


class Model {
public:
	using NumericFunction = std::function<double(const ModelParameters&, double)>;

public:
	Model(const ModelParameters& params, NumericFunction&& phi_0, NumericFunction&& node,
		std::ostream& out, const std::string& details = "---");
	void run();

protected:
	ModelParameters params;
	NumericFunction phi_0;
	NumericFunction node;
	std::ostream& out;
	std::string details;
	double dx;
	double dt;
	std::vector<double> phi;
	std::vector<double> r;
	std::vector<double> r_mid;
	std::vector<double> dr;
	std::vector<double> dr_mid;
	std::vector<double> r_coef;
	std::vector<double> r_coef_mid;
	std::vector<double> phi_r_mid;
	std::vector<double> phi_r_cubed_mid;
	std::vector<double> phi_x_x;
	std::vector<double> phi_x_cubed_x;
	std::vector<double> phi_t;
	ProgressBar progress_bar;

protected:
	static std::string toSingleLine(const std::string&);
	static double toPower(double value, size_t power);
	void initialize();
	double f(double phi) const;
	double f_phi(double phi) const;
	double eps_phi(double phi) const;
	void iterationDerivatives();
	void iterationUpdate();
	void printValues(std::ostream& out, bool are_nodes = false) const;
	virtual std::string getDescription() const;
	virtual bool shallPrint_phi() const;
	virtual void additionalOutput() const;
	virtual bool stopCondition() const;
};