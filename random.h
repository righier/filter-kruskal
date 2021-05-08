#pragma once

#include <stdint.h>
#include "utils.h"

struct Random {
	
	Random(u64 seed) {
		s[0] = init(seed);
		s[1] = init(seed);
	}

	u64 getUint64() {
		return next();
	}

	double getDouble() {
		return (next() >> 11) * (1.0 / (u64(1) << 53));
	}

	float getFloat() {
		return (next() >> 40) * (1.0f / (u32(1) << 24));
	}

	int getInt(int max) {
		return (int)(getDouble() * max);
	}

	int getInt(int min, int max) {
		return getInt(max - min) + min;
	}

	u64 getULong() {
		return next();
	}

	u64 getULong(u64 max) {
		return next() % max;
	}

	u64 getULong(u64 min, u64 max) {
		return min + getULong(max - min);
	}

private:

	u64 s[2];


    /*  Written in 2015 by Sebastiano Vigna (vigna@acm.org)

    To the extent possible under law, the author has dedicated all copyright
    and related and neighboring rights to this software to the public domain
    worldwide. This software is distributed without any warranty.

    See <http://creativecommons.org/publicdomain/zero/1.0/>. */

	u64 init(u64 &x) {
		u64 z = (x += 0x9e3779b97f4a7c15ULL);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
		z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
		return z ^ (z >> 31);
	}

    /*  Written in 2016-2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

    To the extent possible under law, the author has dedicated all copyright
    and related and neighboring rights to this software to the public domain
    worldwide. This software is distributed without any warranty.

    See <http://creativecommons.org/publicdomain/zero/1.0/>. */

	static inline u64 rotl(const u64 x, int k) {
		return (x << k) | (x >> (64 - k));
	}

	u64 next(void) {
		const u64 s0 = s[0];
		u64 s1 = s[1];
        const u64 result = s0 + s1;

		s1 ^= s0;
		s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
		s[1] = rotl(s1, 37);

        return result;
	}

};