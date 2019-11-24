#ifndef CONSTANTS_H
#define CONSTANTS_H

// Value used to calibrate the scale
#define CALIBRATION_FACTOR         -7050

// Weight used to determine that a cat is no longer present
#define CAT_NOT_PRESENT_THRESHOLD   1.0f

// Minimum weight of a cat before it will register
// Must be greater than CAT_NOT_PRESENT_THRESHOLD
#define MIN_CAT_WEIGHT_LBS          4.0f

// Maximum change allowed between readings. If greater than this value,
// assume it is a bad reading and throw it away.
#define MAX_LBS_CHANGE              25.0f

// Number of readings in a row that must match before action is taken
#define NUM_REQ_SAME_READINGS       10

#endif