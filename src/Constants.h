/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Adafruit IO API Key length
#define AIO_KEY_LEN                 32

// Initial calibration of the scale
#define CALIBRATION_FACTOR_INIT    -10850

// Address of the cat database in the EEPROM
#define CAT_DATABASE_ADDR           0x0000

// Magic number used to determine if the EEPROM has been programmed
#define CAT_MAGIC_NUMBER            0xfeedbeef

// Weight used to determine that a cat is no longer present
#define CAT_NOT_PRESENT_THRESHOLD   1.0f

// Number of Grams in a Pound
#define GRAMS_IN_POUND              453.592f

// Flag to indicate if the compiled target is the gateway mesh device
#define IS_MASTER_DEVICE            (PLATFORM_ID != PLATFORM_XENON)

// Minimum weight of a cat before it will register
// Must be greater than CAT_NOT_PRESENT_THRESHOLD
#define MIN_CAT_WEIGHT_LBS          4.0f

// Maximum length of a cat's name
#define MAX_CAT_NAME_LEN            16

// Maximum amount a cat's weight can change between visits
#define MAX_CAT_WEIGHT_CHANGE       0.30f

// Maximum number of milliseconds allowed between readings. If the previous reading
// occurs more than this number of milliseconds in the past, assume it is a bad
// reading and throw it away.
#define MAX_MS_BETWEEN_READINGS     200

// Maximum number of cats that can be stored
#define MAX_NUM_CATS                5

// Pins used to communicate with the HX711
#define PIN_HX711_DOUT              D3
#define PIN_HX711_CLK               D2

// Pin used to toggle the LED on the board
#define PIN_LED                     D7

// Initial value for the number of readings in a row that must match before action is taken
#define READINGS_TO_BE_STABLE_INIT  10

// Address of the scale configuration in the EEPROM
#define SCALE_CONFIG_ADDR           0x0400

// Magic number used to determine if the EEPROM has been programmed
#define SCALE_CONFIG_MAGIC_NUMBER   0x5ca1ab1e

// Baud rate of the debug serial port
#define SERIAL_BAUD                 115200

// Time constant to use to exponentially smooth the scale readings
#define SMOOTH_TIME_CONSTANT        0.5f

// Time in milliseconds to wait before toggling the LED in training state
#define TRAINING_LED_TOGGLE_MS      500

// Flag to indicate if the compiled target supports using Adafruit IO
#define USE_ADAFRUIT_IO             (PLATFORM_ID != PLATFORM_XENON)

#endif