#pragma once

#include <chrono>


class ProgressBar {
public:
	explicit ProgressBar(size_t size, size_t bar_size = 50);
	void update(size_t value);

private:
	size_t size;
	size_t bar_size;
	size_t items = 0;
	bool was_drawn = false;

private:
	size_t getItems(size_t value) const;
	void printSymbol(char symbol, size_t quantity) const;
	void redraw() const;
};