#pragma once

#include "model/model_parameters.h"

#include <iostream>
#include <vector>
#include <string>
#include <functional>


class Model {
public:
	using Function_phi_0 = std::function<double(const ModelParameters&, double)>;

public:
	Model(const ModelParameters& params, Function_phi_0&& phi_0, std::ostream& out,
		const std::string& details = "---");
	void run();

protected:
	ModelParameters params;
	Function_phi_0 phi_0;
	std::ostream& out;
	std::string details;
	double dx;
	double dt;
	std::vector<double> phi;
	std::vector<double> phi_next;

protected:
	static std::string toSingleLine(const std::string&);
	void initialize();
	double f(double phi) const;
	double f_phi(double phi) const;
	double eps_phi(double phi) const;
	void iteration();
	void printValues(std::ostream& out) const;
	virtual std::string getDescription() const;
	virtual bool shallPrint_phi() const;
	virtual void additionalOutput() const;
	virtual bool stopCondition() const;
};