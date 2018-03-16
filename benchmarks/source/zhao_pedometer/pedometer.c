#include "pedometer.h"

#include <math.h>	// for fabs
#include <stdlib.h> // for calloc

// REFERENCE: Zhao, 2010

// Window Filter
wfilt_t *wfilt(size_t size)
{
	wfilt_t *filt = calloc(1, sizeof(wfilt_t));
	filt->n_samples = size;
	filt->samples = calloc(size, sizeof(float));
	return filt;
}

void wfilt_free(wfilt_t *filt)
{
	free(filt->samples);
	free(filt);
}

float wfilt_filter(wfilt_t *filt, float sample)
{
	filt->sample_index = (filt->sample_index + 1) % filt->n_samples;
	filt->samples[filt->sample_index] = sample;
	float sum = 0.f;
	for (size_t i = 0; i < filt->n_samples; ++i)
		sum += filt->samples[i];
	return sum / filt->n_samples;
}

// Min/max pool
pool_t *pool(size_t size, pool_comp_t compare)
{
	pool_t *pool = calloc(1, sizeof(pool_t));
	pool->n_samples = size;
	pool->samples = calloc(size, sizeof(float));
	pool->compare = compare;
	return pool;
}

void pool_free(pool_t *pool)
{
	free(pool->samples);
	free(pool);
}

void pool_put(pool_t *pool, float sample)
{
	pool->sample_index = (pool->sample_index + 1) % pool->n_samples;
	pool->samples[pool->sample_index] = sample;
}

float pool_get(pool_t *pool)
{
	float *samples = pool->samples;
	float best = samples[0];
	for (size_t i = 1; i < pool->n_samples; ++i)
	{
		if (pool->compare(samples[i], best))
			best = samples[i];
	}
	return best;
}

int pool_comp_lessthan(float a, float b)
{
	return a < b;
}

int pool_comp_greaterthan(float a, float b)
{
	return a > b;
}

// Pedometer constructor/destructor
pedometer_t *pedometer(
	int lowpass_size,
	int threshold_window_size,
	float sensitivity,
	float step_period_min,
	float step_period_max,
	int steps_to_sync)
{
	pedometer_t *ped = calloc(1, sizeof(pedometer_t));
	for (size_t i = 0; i < PEDOMETER_N_AXES; ++i)
	{
		ped->lowpass_filter[i] = wfilt(lowpass_size);
		ped->axis_min_pool[i] = pool(threshold_window_size, pool_comp_lessthan);
		ped->axis_max_pool[i] = pool(threshold_window_size, pool_comp_greaterthan);
	}
	ped->threshold_window_size = threshold_window_size;
	ped->sensitivity = sensitivity;
	ped->step_period_min = step_period_min;
	ped->step_period_max = step_period_max;
	ped->sync_state_max = steps_to_sync;

	return ped;
}

void pedometer_free(pedometer_t *ped)
{
	for (size_t i = 0; i < PEDOMETER_N_AXES; ++i)
	{
		wfilt_free(ped->lowpass_filter[i]);
		pool_free(ped->axis_min_pool[i]);
		pool_free(ped->axis_max_pool[i]);
	}
	free(ped);
}

// Main algorithm implementation
// Returns a step increment >= 0
int pedometer_put(pedometer_t *ped, float delta_t, float x, float y, float z)
{
	ped->time_since_step += delta_t;

	// Filter acceleration, and update pools with filtered values
	float accel[PEDOMETER_N_AXES] = {x, y, z};
	for (size_t i = 0; i < PEDOMETER_N_AXES; ++i)
	{
		accel[i] = wfilt_filter(ped->lowpass_filter[i], accel[i]);
		pool_put(ped->axis_min_pool[i], accel[i]);
		pool_put(ped->axis_max_pool[i], accel[i]);
	}

	// Update dynamic threshold if it's time to
	ped->sample_count++;
	if (ped->sample_count >= ped->threshold_window_size)
	{
		ped->sample_count = 0;
		float greatest_range = -1;
		for (size_t i = 0; i < PEDOMETER_N_AXES; ++i)
		{
			float max = pool_get(ped->axis_max_pool[i]);
			float min = pool_get(ped->axis_min_pool[i]);
			if (max - min > greatest_range)
			{
				greatest_range = max - min;
				ped->current_active_axis = i;
				ped->dynamic_threshold = (min + max) / 2.f;
			}
		}
	}

	// Transition and periodicity detection
	ped->sample_old = ped->sample_new;
	if (fabs(accel[ped->current_active_axis] - ped->prev_accel[ped->current_active_axis]) >= ped->sensitivity)
		ped->sample_new = accel[ped->current_active_axis];

	int transition_detected =
		ped->sample_old > ped->dynamic_threshold &&
		ped->sample_new < ped->dynamic_threshold;
	int transition_valid =
		ped->time_since_step >= ped->step_period_min &&
		ped->time_since_step <= ped->step_period_max;
	int rv = 0;

	if (transition_detected && transition_valid)
	{
		ped->time_since_step = 0;
		if (ped->sync_state == ped->sync_state_max)
			// Synchronised steady state
		{
			rv = 1;
		}
		else if (ped->sync_state == ped->sync_state_max - 1)
		{
			// Entering synchronisation
			// As well as this transition, we also need to account for those counted whilst reaching synchronised state:
			rv = ped->sync_state_max;
			ped->sync_state++;
		}
		else
		{
			ped->sync_state++;
		}
	}
	else if (transition_detected && !transition_valid)
	{
		ped->time_since_step = 0;
		ped->sync_state = 0;
	}

	// Update any remaining state variables
	for (size_t i = 0; i < PEDOMETER_N_AXES; ++i)
	{
		ped->prev_accel[i] = accel[i];
	}

	return rv;
}
