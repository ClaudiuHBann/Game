#pragma once

#include <random>
using namespace std;

class Random {
public:
    Random();

    template<typename Integer>
    Integer GetInteger(
        const Integer min = numeric_limits<Integer>::min(),
        const Integer max = numeric_limits<Integer>::max()
    ) {
        return uniform_int_distribution(min, max)(mEngine);
    }

    template<typename Real>
    Real GetReal(
        const Real min = numeric_limits<Real>::min(),
        const Real max = numeric_limits<Real>::max()
    ) {
        return uniform_real_distribution(min, max)(mEngine);
    }

private:
    random_device mGenerator;
    mt19937 mEngine;
};