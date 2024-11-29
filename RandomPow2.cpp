#include "RandomPow2.h"
#include <cstdint>
#include <random>

#define POW2(x) (1LL << (x))
namespace RandomPow2 {
	std::mt19937 gen(0 /* seed */);

	long long int generate_random_pow2(long long int min, long long int max) { // max is inclusive, so if max is 64 then 64 will be generated
		uint8_t min_exp = 0, max_exp = 0;
		while (POW2(min_exp) < min) {
			min_exp++;
		}
		while (POW2(max_exp + 1) <= max) { // if 2^(max_exp + 1) still falls under the range
			max_exp++;
		}
		
		std::uniform_int_distribution<long long> distr(min_exp, max_exp);
		return POW2(distr(gen));
	}
}
