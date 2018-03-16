import numpy as np
from random import random

def steps(n_dim, size):
	for i in range(n_dim):
		yield np.array([0] * i + [ size] + [0] * (n_dim - i - 1))
		yield np.array([0] * i + [-size] + [0] * (n_dim - i - 1))

def tabu_search(f, x0, xmin, xmax, step_initial, step_final, step_factor, threshold_intensify=10, threshold_diversify=20, threshold_reduce=30, stm_size=10, mtm_size=5):
	step_size = step_initial
	x = np.array(x0)
	global_min = f(x0)
	best_x = x
	stm = []
	mtm = []
	n_dim = len(x0)
	steps_since_min = 0
	while step_size >= step_final:
		accepted = None
		for step in steps(n_dim, step_size):
			if list(x + step) in (list(x[1]) for x in stm):
				continue
			cost = f(x + step)
			if accepted is None or cost < accepted[0]:
				accepted = (cost, x + step)
		if accepted is not None:
			#print("Accepting {}".format(accepted))
			#print("{} steps since global min".format(steps_since_min))
			stm = [accepted] + stm[:stm_size-1]
			mtm = sorted(mtm + [accepted], key=lambda x: x[0])[:mtm_size]
			if accepted[0] < global_min:
				steps_since_min = 0
				global_min = accepted[0]
				best_x = accepted[1]
				continue
		steps_since_min += 1
		if steps_since_min == threshold_reduce:
			step_size = step_size / step_factor
			steps_since_min = 0
			continue
		elif steps_since_min == threshold_diversify:
			x = [random() * (xmax - xmin) + xmin for i in range(n_dim)]
		elif steps_since_min == threshold_intensify:
			x = sum(x[1] for x in mtm) / len(mtm)
	return (global_min, best_x)