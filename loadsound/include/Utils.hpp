#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <cmath>
#include <stdio.h>

template<typename T> T log10scale(double x, T min, T max) {
    double factor = log10((double)max / min);
    return min * pow( 10, ( x * factor ) );
}

template<typename T> T log10scale_reverse(double x, T min, T max) {
    return log10(x / min) / log10((double) max / min);
}

// Log scaling f(x, width) = 20 * 10^3(x / width)
// Reverse:    x(f, width) = (max / 3)*math.log10(f / 20)
double x_to_freq(double x, double width) {
    //return (width * pow(10, x)) - width;
    return 20 * pow(10, 3*x);
}

double f_to_x(double f, double width) {
    return (width/3) * log10(f / 20);
}

#endif
