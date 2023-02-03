#pragma once
#include <string>

typedef struct t_random {
	unsigned __int64 seed;

	unsigned __int64 get_Seed();
	void set_Seed(unsigned __int64 new_seed);

	unsigned __int64 random_seed();
	bool get_proctype();
	int get_less(int min, int max);
} g_random;