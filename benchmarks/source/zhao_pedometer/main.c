#include <stdio.h>

#include "sf-types.h"
#include "devscc.h"
#include "devsim7708.h"
#include "pedometer.h"
#include "sh7708.h"

enum
{
	SENS_ACCELX   = 0,
	SENS_ACCELY   = 1,
	SENS_ACCELZ   = 2,
	SENS_ACTIVITY = 3
};

float sensor_read(int which)
{
	return *(float*)((void*)SENSOR_READ + which * SUPERH_SENSOR_REG_SPACING);
}

int main(void)
{
	printf("\n");
	printf("---------------------\n");
	printf(" Simulation starting \n");
	printf("---------------------\n");
	
	pedometer_t *ped = pedometer(4, 50, 0.01f, 0.2f, 2.0f, 4);
	float t = 0;
	float delta_t = 0.02f;
	
	for (size_t i = 0; i < 1000; ++i)
	{
		t += delta_t;
		float x = t - (int)t;
		int step_increment = pedometer_put(ped, delta_t, x, 0, 0);
		printf("(%f, %f, %d),\n", t, x, step_increment);
	}

	pedometer_free(ped);
	
	return 0;
}
