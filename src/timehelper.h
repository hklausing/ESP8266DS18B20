/*-----------------------------------------------------------
NTP & time routines for ESP8266
    for ESP8266 adapted Arduino IDE
by Stefan Thesen 05/2015 - free for anyone
code for ntp adopted from Michael Margolis
code for time conversion based on http://stackoverflow.com/
-----------------------------------------------------------*/

#pragma once

// note: all timing relates to 01.01.2000

#include <Arduino.h>
#include <time.h>
#include "timers.hpp"


String convertEpochToIso8601(const time_t epoch);


extern timer_values_t g_timer_values;