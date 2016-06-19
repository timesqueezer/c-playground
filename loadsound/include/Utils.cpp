#include <cmath>

#include "Utils.hpp"

template<typename T> T log10scale(T x, T min, T max) {
    /****
     * Transforms an input x: 0 <= x <= 1
     * returns: position of x on a logarithmic scale raning from min to max
     * f(x) = min * 10^(x*lg(max/min))
     */
    return min * pow( 10, ( x * log10(max / min) ) );
}