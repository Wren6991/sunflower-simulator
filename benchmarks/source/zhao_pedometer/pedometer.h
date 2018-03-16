#ifndef _PEDOMETER_H_
#define _PEDOMETER_H_

#include <stddef.h>

#define PEDOMETER_N_AXES 3

// Window lowpass filter

typedef struct wfilt
{
	size_t n_samples;
	int sample_index;
	float *samples;
} wfilt_t;

wfilt_t *wfilt(size_t size);
void wfilt_free(wfilt_t *filt);
float wfilt_filter(wfilt_t *filt, float sample);

// Min/max pool type (or any other relational operator)

typedef int (*pool_comp_t)(float, float);

typedef struct pool
{
	size_t n_samples;
	int sample_index;
	float *samples;
	pool_comp_t compare;
} pool_t;

int pool_comp_lessthan(float a, float b);
int pool_comp_greaterthan(float a, float b);

// Main pedometer object

typedef struct pedometer
{
	// Subcomponents
	wfilt_t *lowpass_filter[PEDOMETER_N_AXES];
	pool_t *axis_max_pool[PEDOMETER_N_AXES];
	pool_t *axis_min_pool[PEDOMETER_N_AXES];
	// Configuration
	int threshold_window_size;
	float sensitivity;
	float step_period_min;
	float step_period_max;
	// State
	float prev_accel[PEDOMETER_N_AXES];	// previous filtered accelerometer values.
	float dynamic_threshold;
	float sample_new;
	float sample_old;
	float time_since_step;
	int sync_state_max;
	int sync_state;	// max if synchronised, < max if not synchronised, reset to 0 on invalid step
	int sample_count;
	int current_active_axis;
} pedometer_t;

pedometer_t *pedometer(int lowpass_size, int threshold_window_size, float sensitivity, float step_period_min, float step_period_max, int steps_to_sync);
void pedometer_free(pedometer_t *ped);
int pedometer_put(pedometer_t *ped, float delta_t, float x, float y, float z);	// return 1 if step

#endif // _PEDOMETER_H_	

