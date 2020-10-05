/**
 * file         settings.hpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-09-21
 * Note         Defines project specific values
 */

#pragma once

#include <Arduino.h>

/*
 * Software definitions
 */
#define SOFTWARE_VERSION "v0.1.0"
#define RELEASE_DATE "2020-10-05"
#define SOFTWARE_AUTHOR "Heiko Klausing"
/// comment the following line to disable the OTA feature
#define ARDUINO_OTA_ENABLE

/*
 * Temperature logger information
 */
#define ESP8266_HOSTNAME "TempLog02"
#define ESP8266_LOCATION "TestObject"


/*
 * Hardware definitions
 */

/// LED to signalize the measurement activity; LED on ESP8266
constexpr uint8_t LED_PIN_MEASACTIVITY = D4;

/// LED to signalize the Web Page access; LED NodeMCU
constexpr uint8_t LED_PIN_WEBPAGES = D0;

/// GPIO to access the 1-wire devices
constexpr uint8_t ONE_WIRE_PIN = D3;

/// Start memory address of EEPROM usage
constexpr int8 eeprom_startAddress = 0;

/*
 * Web Server
 */

/// Web Server Port number
constexpr uint16_t WEB_SERVER_PORT = 80;
/// Amount of bytes per block for transmitting -> reduce required RAM size
constexpr uint32_t HTTP_BLOCK_SIZE = 1024;

/*
 * Sensor
 */

// /// Temperature device correction offset
// constexpr float g_correction = -1.0;
/// Buffer size of DS18B20 serial code
constexpr size_t SERIAL_CODE_SIZE = (64 /*bit*/ / 4 /*nibble size*/) + 1 /*Termination*/;

/*
 * Size of the ringbuffer for the measurement results.
 */
#define TIME_MEASUREMENTS_PER_HOUR 10 // per hour
#define TIME_HOURS_PER_DAY         24 // hours
#define TIME_DOMAIN_IN_DAYS        14 // days
#define TIME_MEASUREMENT_DISTANCE  15 // seconds

/// Amount of measurment elements that have to be stored in the queue
constexpr size_t RINGBUFFER_SIZE = TIME_MEASUREMENTS_PER_HOUR  * TIME_HOURS_PER_DAY  * TIME_DOMAIN_IN_DAYS ;
/// time domain that defines the time distance in sec to store the next measurement value to queue
constexpr uint32_t MEASURMENT_DOMAIN = 60 * 60 / TIME_MEASUREMENTS_PER_HOUR;

/*
 * Parameter definitions
 */

/// size of string size in parameter list incluing termination
constexpr int STRING_SIZE = 32;
/// if the parameter list is changed this values should be changed
constexpr int PARAMETER_LIST_VERSION = 1;
