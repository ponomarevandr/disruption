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
	struct InterCoefficients {
		double a_first;
		double a_second;
		double b_first;
		double b_second;

		InterCoefficients() = default;
		InterCoefficients(double a_first, double a_second, double b_first, double b_second);
	};

	enum class InterCase {
		QUADRATIC,
		CUBIC,
		POWER_TWO_THIRDS,
		LOGARITHMIC,
		POWER_ONE_THIRD
	};

protected:
	ModelParameters params;
	NumericFunction phi_0;
	NumericFunction node;
	std::ostream& out;
	std::string details;
	double dx;
	double dt;
	std::vector<double> phi;
	std::vector<double> r_border;
	std::vector<double> s_border;
	std::vector<double> dv;
	std::vector<double> r;
	std::vector<double> phi_grad_border;
	std::vector<double> phi_lapl;
	std::vector<double> phi_lapl_grad_border;
	std::vector<double> flow_density_border;
	std::vector<double> flow_border;
	std::vector<double> phi_t;
	std::vector<double> inter_coef_a_first_border;
	std::vector<double> inter_coef_a_second_border;
	InterCoefficients inter_coefs_higher;
	InterCase inter_case;
	double inter_a_higher;
	double inter_b_higher;
	ProgressBar progress_bar;

protected:
	static std::string toSingleLine(const std::string&);
	static double toPower(double value, size_t power);
	double getRPowerDiff(size_t index, size_t power) const;
	InterCoefficients getInterCoefficients(size_t index, double integral_a_first,
		double integral_a_second, double integral_b_first, double integral_b_second) const;
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
	virtual void iterationAdditional();
};