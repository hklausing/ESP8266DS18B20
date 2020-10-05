/*
 * File         led.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-05
 * Note         Handles functions around LEDs
 */

#pragma once


class Led
{
private:
    /* data */
    uint8_t m_pin;

public:
    Led(uint8_t pin);
    ~Led();

    uint8_t status(void);

    void ledOff(void);

    void ledOn(void);

    void toggleLed(void);
};
