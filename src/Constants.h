#ifndef CONSTANTS_H
#define CONSTANTS_H

// Value used to calibrate the scale
#define CALIBRATION_FACTOR         -7050

// Address of the cat database in the EEPROM
#define CAT_DATABASE_ADDR           0x0000

// Magic number used to determine if the EEPROM has been programmed
#define CAT_MAGIC_NUMBER            0xfeedbeef

// Weight used to determine that a cat is no longer present
#define CAT_NOT_PRESENT_THRESHOLD   1.0f

// Minimum weight of a cat before it will register
// Must be greater than CAT_NOT_PRESENT_THRESHOLD
#define MIN_CAT_WEIGHT_LBS          4.0f

// Maximum length of a cat's name
#define MAX_CAT_NAME_LEN            16

// Maximum change allowed between readings. If greater than this value,
// assume it is a bad reading and throw it away.
#define MAX_LBS_CHANGE              25.0f

// Maximum number of cats that can be stored
#define MAX_NUM_CATS                5

// Number of readings in a row that must match before action is taken
#define NUM_REQ_SAME_READINGS       10

#endif