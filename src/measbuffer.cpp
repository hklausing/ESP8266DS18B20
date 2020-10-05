/*
 * File         src/measbuffer.cpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-09-22
 * Note         handles the measurement buffer
 */

#include "settings.hpp"
#include "measbuffer.hpp"


measValue_t g_measvalue = {0, 0.0};

RingBuffer<measValue_t, RINGBUFFER_SIZE> g_ringbuffer(g_measvalue);
