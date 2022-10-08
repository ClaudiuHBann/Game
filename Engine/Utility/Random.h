#pragma once

class Random {
public:
	Random();

	template<typename Integer>
	inline Integer GetInteger(
		const Integer min = numeric_limits<Integer>::min(),
		const Integer max = numeric_limits<Integer>::max()
	) {
		return uniform_int_distribution(min, max)(mEngine);
	}

private:
	random_device mGenerator;
	mt19937 mEngine;
};