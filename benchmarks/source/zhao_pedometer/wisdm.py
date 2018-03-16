import os.path
import numpy as np

base_path = "../data/WISDM_ar_v1.1"
wisdm_ar_raw = os.path.join(base_path, "WISDM_ar_v1.1_raw.txt")

def drop(iter, offset):
	for i, data in enumerate(iter):
		if i >= offset:
			yield data

def iter_pairs(iter):
	for pair in zip(iter, drop(iter, 1)):
		yield pair

class Dataset:
	def __init__(self, id, n_num_vars=1, n_tag_vars=0):
		self.id = id
		self.times = []
		self.data = [[] for i in range(n_num_vars)]
		self.tags = [[] for i in range(n_tag_vars)]

	def arr_to_numpy(self):
		for i in range(len(self.data)):
			self.data[i] = np.array(self.data[i])
		self.times = np.array(self.times)

	def compressed_timebase(self, max_gap_size):
		offset = self.times[0]
		yield 0
		for time, prevtime in iter_pairs(self.times):
			if time - prevtime > max_gap_size:
				offset += time - prevtime - max_gap_size
			yield time - offset

	@staticmethod
	def interp(d1, d2, t):
		return tuple(d1[i] + t * (d2[i] - d1[i]) * (1 - t) for i in range(len(d1)))

	def resample(self, period, max_gap_size):
		idx = 0
		t_sample = self.times[0]
		data = iter_pairs(zip(*self.data))
		print(len(self.times))
		sentinel = object()
		for t1, t2 in iter_pairs(self.times):
			d1, d2 = next(data)
			assert(t1 < t2)
			while t_sample < t2:
				yield self.interp(d1, d2, (t_sample - t2) / (t2 - t1))
				t_sample += period

	def write_sunflower(self, files):



# Occasionally, WISDM contains consecutive samples with duplicate timestamps,
# which breaks interpolation (t2 - t1 == 0  =>  division by 0).
# Since this is unphysical anyway, drop these samples.
def remove_duplicate_times(ds):
	ds_new = Dataset(ds.id, len(ds.data), len(ds.tags))
	lastt = -1
	for i, t in enumerate(ds.times):
		if t != lastt:
			ds_new.times.append(t)
			for j in range(len(ds.data)):
				ds_new.data[j].append(ds.data[j][i])
			for j in range(len(ds.tags)):
				ds_new.tags[j].append(ds.tags[j][i])
		lastt = t
	return ds_new

# Read in CSV of the format (user, activity, timestamp, x_accel, y_accel, z_accel)
# Return a record set for each user
# There are some edge cases to account for malformed CSV in standard dataset
def load_wisdm(path=wisdm_ar_raw):
	users = dict()
	for line in open(path).readlines():
		if ";" not in line:
			continue
		lasttimestamp = 0
		for record in line.strip(";\n").split(";"):
			user, activity, timestamp, x_accel, y_accel, z_accel = tuple(record.strip(",").split(","))
			user = int(user)
			if user not in users:
				users[user] = Dataset(user, n_num_vars=3, n_tag_vars=1)
			ds = users[user]
			if int(timestamp) == 0:
				# This seems to be an occasional fault with the dataset (e.g. on line 684891)
				# Chuck it away
				continue
			lasttimestamp = float(timestamp)
			ds.times.append(float(timestamp) / 1e9)
			ds.data[0].append(float(x_accel))
			ds.data[1].append(float(y_accel))
			ds.data[2].append(float(z_accel))
			ds.tags[0].append(activity)
	return dict((u, remove_duplicate_times(d)) for u, d in users.items())

if __name__ == "__main__":
	ds = load_wisdm()
