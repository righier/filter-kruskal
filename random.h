#include <stdint.h>


struct Random {

	Random(uint64_t seed) {
		s[0] = init(seed);
		s[1] = init(seed);
	}

	uint64_t getUint64() {
		return next();
	}

	double getDouble() {
		return (next() >> 11) * (1.0 / (uint64_t(1) << 53));
	}

	float getFloat() {
		return (next() >> 40) * (1.0f / (uint32_t(1) << 24));
	}

	int getInt(int max) {
		return (int)(getDouble() * max);
	}

	int getInt(int min, int max) {
		return getInt(max - min) + min;
	}

private:

	uint64_t s[2];

	uint64_t init(uint64_t &x) {
		uint64_t z = (x += 0x9e3779b97f4a7c15);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
		return z ^ (z >> 31);
	}

	static inline uint64_t rotl(const uint64_t x, int k) {
		return (x << k) | (x >> (64 - k));
	}

	uint64_t next(void) {
		const uint64_t s0 = s[0];
		uint64_t s1 = s[1];

		s1 ^= s0;
		s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
		s[1] = rotl(s1, 37);

		return s0 + s1;
	}

};