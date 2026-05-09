#pragma once

#include <cstddef>


namespace Utils {

template<size_t EXP>
inline double powerNatural(double value) {
	double result = powerNatural<(EXP >> 1)>(value);
	return result * result * powerNatural<EXP & size_t{1}>(value);
}

template<>
inline double powerNatural<1>(double value) {
	return value;
}

template<>
inline double powerNatural<0>(double value) {
	return 1.0;
}

}