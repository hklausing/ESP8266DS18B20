/*
 * File         src/measbuffer.hpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-09-22
 * Note         handles the measurement buffer
 */

#pragma once

#include "localtime.h"
#include "settings.hpp"

#include "miniringbuffer.hpp"


// Rinbuffer
// static const size_t RINGBUFFER_SIZE = 10 /*per hour*/ * 24 * 14/*days*/;
typedef struct {
    time_t timestamp;
    float temperature;
} measValue_t;

extern measValue_t g_measvalue;

extern RingBuffer<measValue_t, RINGBUFFER_SIZE> g_ringbuffer;
