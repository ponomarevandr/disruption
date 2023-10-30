#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>


struct ModelParameters {
	double eps_0;
	double delta;
	double l;
	double Gamma;
	double m;
	double Phi_coefficient;
	double dx;
	size_t x_skip;
	double dt;
	size_t t_skip;
	double width;
	double duration;

	void setPhysical(double eps_0, double delta, double l, double Gamma, double m,
		double Phi_coefficient);
	void setComputational(double dt, size_t t_skip, double dx, size_t x_skip);
	void setSizes(double duration, double width);
	void print(std::ostream& out) const;
	void read(std::istream& in);

	template <typename T>
	static T fromString(const std::string& string) {
		std::stringstream sstream(string);
		T result;
		sstream >> result;
		return result;
	}
	static std::vector<std::string> split(const std::string& text);
};