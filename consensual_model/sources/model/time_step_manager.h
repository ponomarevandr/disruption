#pragma once

#include <cstddef>


class TimeStepManager {
private:
	size_t t_grid;
	double dt;
	size_t step_index = 0;
	size_t level = 0;

public:
	TimeStepManager() = default;
	TimeStepManager(size_t t_grid, double dt);
	void nextStep(double dt_adaptive);
	size_t getLevel() const;
	size_t getScale() const;
};