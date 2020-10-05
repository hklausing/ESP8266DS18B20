/*
 * File         led.cpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-05
 * Note         Handles functions around LEDs
 */

#include <Arduino.h>
#include "led.h"


Led::Led(uint8_t pin)
    : m_pin{pin}
{
    pinMode(m_pin, OUTPUT);
    ledOff();
}

Led::~Led(){}


uint8_t Led::status(void)
{
    return digitalRead(m_pin);
}

void Led::toggleLed(void)
{
    digitalWrite(m_pin, !digitalRead(m_pin));
}

void Led::ledOn(void)
{
    digitalWrite(m_pin, 0);
}

void Led::ledOff(void)
{
    digitalWrite(m_pin, 1);
}
