/*
 * File         meas.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-06
 * Note         Measures temperature value and calculates an average value
 */

#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

typedef uint8_t SerialCode_t[8];


class Measurement
{
private:
    /// 1-wire device ID    
    const uint8_t m_device_ID = 0;

    /* data */
    float m_average_collector;
    int m_average_counter;
    float m_correction; // measured temperature value correction
    float m_last_scan_value;

    /// Address of the first device
    SerialCode_t m_serialcode;

    // Setup a oneWire instance to communicate with any OneWire devices
    OneWire *m_onewire;

    // Pass our oneWire reference to Dallas Temperature sensor
    DallasTemperature *m_ds18b20;

public:
    Measurement(uint8_t pin);

    ~Measurement();

    void meas(void);

    float getValue(void);

    void restartAverage(void);

    void setCorrection(const float correction);

    const char* getSerialCode(void);
};


// temperature measurement via OneWire and DS18B20
extern Measurement g_temp_meas;
