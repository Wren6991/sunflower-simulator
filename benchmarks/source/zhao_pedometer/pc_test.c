// Simple (PC) test program for pedometer.c
// Just build with gcc pc_test.c pedometer.c -lm -o ped
// with your machine's local GCC

#include "pedometer.h"

#include <stdio.h>
#include <math.h>

#define DELTA_T 0.02f

int main(int argc, char **argv)
{
	pedometer_t *ped = pedometer(4, 50, 0.01f, 0.2f, 2.0f, 4);

	float t = 0.f;

	for (size_t i = 0; i < 1000; ++i)
	{
		t += DELTA_T;
		float x = sin(10.f * t);
		int step_increment = pedometer_put(ped, DELTA_T, sin(10.f * t), 0, 0);
		printf("(%f, %f, %d),\n", t, x, step_increment);
	}

	pedometer_free(ped);
	return 0;
}