/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Adafruit IO API Key length
#define AIO_KEY_LEN                 32

// Time constant used to exponentially smooth battery percentage readings
#define BATT_PERCENT_TIME_CONSTANT  5.0f

// Maximum voltage of the battery
#define BATTERY_MAX_VOLT            4.1f

// Minimum voltage of the battery
#define BATTERY_MIN_VOLT            3.6f

// Battery percentage to warn the user of low battery
#define BATTERY_WARN_PERCENT        20

// Initial calibration of the scale
#define CALIBRATION_FACTOR_INIT    -10850

// Address of the cat database in the EEPROM
#define CAT_DATABASE_ADDR           0x0000

// Magic number used to determine if the EEPROM has been programmed
#define CAT_MAGIC_NUMBER            0xfeedbeef

// Weight used to determine that a cat is no longer present
#define CAT_NOT_PRESENT_THRESHOLD   1.0f

// Time in milliseconds to wait in CAT_POSSIBLE state with a stable reading
#define CAT_POSSIBLE_TIMEOUT_MS     30000

// Number of Grams in a Pound
#define GRAMS_IN_POUND              453.592f

// Minimum weight of a cat before it will register
// Must be greater than CAT_NOT_PRESENT_THRESHOLD
#define MIN_CAT_WEIGHT_LBS          4.0f

// Maximum length of a cat's name
#define MAX_CAT_NAME_LEN            16

// Maximum amount a cat's weight can change between visits
#define MAX_CAT_WEIGHT_CHANGE       0.30f

// Maximum allowed scale drift in grams
#define MAX_DRIFT_GRAMS             8.0f

// Maximum number of milliseconds allowed between readings. If the previous reading
// occurs more than this number of milliseconds in the past, assume it is a bad
// reading and throw it away.
#define MAX_MS_BETWEEN_READINGS     200

// Maximum number of cats that can be stored
#define MAX_NUM_CATS                5

// Maximum time allowed in CAT_PRESENT state. Used to detect situation where amount
// of litter added matches a cat's weight.
#define MAX_CAT_PRESENT_TIME_MS     600000

// Time to delay when idle before disabling network connection
#define NETWORK_DISABLE_DELAY_MS    60000

// Time (in seconds) allowed between cat visits before alert is sent.
// Alert is disabled if value is 0.
#define NO_VISIT_ALERT_TIME_INIT    0

// Pin to pull up when debug mode is desired
#define PIN_DEBUG_MODE              D5

// Pins used to communicate with the HX711
#define PIN_HX711_DOUT              D3
#define PIN_HX711_CLK               D2

// Pin used to toggle the LED on the board
#define PIN_LED                     D7

// Initial value for the number of readings in a row that must match before action is taken
#define READINGS_TO_BE_STABLE_INIT  10

// Used to disable the LED. Not defined by DeviceOS for some reason.
#define RGB_COLOR_NONE              0x00000000

// Address of the scale configuration in the EEPROM
#define SCALE_CONFIG_ADDR           0x0400

// Magic number used to determine if the EEPROM has been programmed
#define SCALE_CONFIG_MAGIC_NUMBER   0x5ca1ab1e

// Baud rate of the debug serial port
#define SERIAL_BAUD                 115200

// Time constant used to exponentially smooth the weight readings
#define SMOOTH_TIME_CONSTANT        0.5f

// Flag to indicate whether the device should stay connected when USB powered
#define STAY_CONNECTED_WHEN_ON_USB  (1)

// Time will be sync'd with the cloud if last sync was at least this long ago in milliseconds
#define TIME_SYNC_MILLIS            (24 * 60 * 60 * 1000)

// Time in milliseconds to wait before toggling the LED in training state
#define TRAINING_LED_TOGGLE_MS      500

// Ultra Low Power (ULP) sleep duration
#define ULP_DURATION_MS             5000

// Flag to indicate if the compiled target supports using Adafruit IO
#define USE_ADAFRUIT_IO             (1)

#endif