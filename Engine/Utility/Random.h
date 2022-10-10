#pragma once

class Random {
public:
	inline Random() : mEngine(mGenerator()) {}

	template<typename Integer>
	inline Integer GetInteger(
		const Integer min = numeric_limits<Integer>::min(),
		const Integer max = numeric_limits<Integer>::max()
	) const {
		return uniform_int_distribution(min, max)(mEngine);
	}

	template<typename Real>
	inline Real GetReal(
		const Real min = numeric_limits<Real>::min(),
		const Real max = numeric_limits<Real>::max()
	) const {
		return uniform_real_distribution(min, max)(mEngine);
	}

private:
	random_device mGenerator;
	mt19937 mEngine;
};