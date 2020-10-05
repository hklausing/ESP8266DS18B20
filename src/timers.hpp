/*
 * File         src/timers.hpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-09-28
 * Note         Timer ans more
 */

#pragma once

#include <Arduino.h>


// timer & counter variables
typedef struct {
    uint32_t now;               // current time [ms]
    uint32_t meas_interval;     // time [ms] for next temperature measurement
    uint32_t next_meas_temp;    // for next temperature measurement time [ms]
    uint32_t store_interval;    // interval time to store averaged mesurement values, time [ms]
    uint32_t next_store_temp;   // for next temperature store time [ms]
    time_t   start_timestamp;   // stores the time of the temperature logger start
} timer_values_t;

