#include "time_step_manager.h"

#include <iostream>


TimeStepManager::TimeStepManager(size_t t_grid, double dt): t_grid(t_grid), dt(dt) {}

void TimeStepManager::nextStep(double dt_adaptive) {
	step_index += 1ull << level;
	while (
		dt * (1ull << (level + 1)) <= dt_adaptive &&
		(step_index & ((1ull << (level + 1)) - 1)) == 0 &&
		step_index + (1ull << (level + 1)) <= t_grid
	) {
		++level;
	}
	while (
		dt * (1ull << level) > dt_adaptive ||
		step_index + (1ull << level) > t_grid
	) {
		--level;
	}
}

size_t TimeStepManager::getLevel() const {
	return level;
}

size_t TimeStepManager::getScale() const {
	return 1ull << level;
}