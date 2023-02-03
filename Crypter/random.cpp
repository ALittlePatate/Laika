#include "random.hpp"
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stdio.h>
#include <random>
#include <Windows.h>
#include "config.hpp"

g_random random;

unsigned __int64 t_random::get_Seed() {
	return seed;
}

void t_random::set_Seed(unsigned __int64 new_seed) {
	seed = new_seed;
	srand((unsigned int)seed);
}

// Robert Jenkins' 96 bit Mix Function
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
	a = a - b;  a = a - c;  a = a ^ (c >> 13);
	b = b - c;  b = b - a;  b = b ^ (a << 8);
	c = c - a;  c = c - b;  c = c ^ (b >> 13);
	a = a - b;  a = a - c;  a = a ^ (c >> 12);
	b = b - c;  b = b - a;  b = b ^ (a << 16);
	c = c - a;  c = c - b;  c = c ^ (b >> 5);
	a = a - b;  a = a - c;  a = a ^ (c >> 3);
	b = b - c;  b = b - a;  b = b ^ (a << 10);
	c = c - a;  c = c - b;  c = c ^ (b >> 15);
	return c;
}

unsigned __int64 t_random::random_seed() {
	srand((unsigned long)mix(clock(), (unsigned long)time(NULL), GetCurrentProcessId())); //http://web.archive.org/web/20070111091013/http://www.concentric.net/~Ttwang/tech/inthash.htm
	unsigned __int64 upper = (unsigned __int64)rand() << 32; // generate a random upper 32 bits
	unsigned __int64 lower = (unsigned __int64)rand(); // generate a random lower 32 bits
	unsigned __int64 result = upper | lower; // combine the upper and lower bits

	return result;
}

bool t_random::get_proctype() {
	return (rand() % 100) < PROB_ANSI;
}

int t_random::get_less(int min, int max) {
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(min, max);

	return dis(gen);
}