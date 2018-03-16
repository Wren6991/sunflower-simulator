Pedometer
=========

1. Portable C implementation of Neil Zhao's pedometer algorithm ([PDF of the paper](reference.pdf)).
2. Some scripts for converting the [WISDM](http://www.cis.fordham.edu/wisdm/dataset.php) accelerometer data set into sunflower `svt` format, and C headers containing constant arrays
3. A simple embedded program, using an activity-based adaptive sampling rate to subsample WISDM data via sunflower sensor interfaces, and outputting a step count
4. A program which uses a nonlinear optimiser to design the sample rate tables, minimising average sample rate subject to step count accuracy constraints

