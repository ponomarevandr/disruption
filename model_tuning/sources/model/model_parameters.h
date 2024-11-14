#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>


struct ModelParameters {
	static constexpr size_t PARAMETERS_NUMBER = 12;
	double eps_0;
	double delta;
	double l;
	double Gamma;
	double m;
	double Phi_gradient;
	size_t x_grid;
	size_t x_skip;
	size_t t_grid;
	size_t t_skip;
	double width;
	double duration;

	void setPhysical(double eps_0, double delta, double l, double Gamma, double m,
		double Phi_gradient);
	void setComputational(size_t x_grid, size_t x_skip, size_t t_grid, size_t t_skip);
	void setSizes(double width, double duration);
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