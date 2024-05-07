#include "progress_bar.h"

#include <iostream>
#include <algorithm>


ProgressBar::ProgressBar(size_t size, size_t bar_size): size(size), bar_size(bar_size) {}

void ProgressBar::update(size_t value) {
	size_t items_new = getItems(value);
	if (items != items_new || !was_drawn) {
		items = items_new;
		was_drawn = true;
		redraw();
	}
}

size_t ProgressBar::getItems(size_t value) const {
	return std::min(bar_size, value * bar_size / size);
}

void ProgressBar::printSymbol(char symbol, size_t quantity) const {
	for (size_t i = 0; i < quantity; ++i) {
		std::cout << symbol;
	}
}

void ProgressBar::redraw() const {
	std::cout << "\r[";
	printSymbol('|', items);
	printSymbol(' ', bar_size - items);
	std::cout << "]  " << items * 100 / bar_size << "%";
	if (items == bar_size)
		std::cout << "\nDone!\n";
	std::cout.flush();
}