/*
 * File         meas.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-06
 * Note         Measures temperature value and calculates an average value
 */

#include "meas.h"
#include "macros.h"
#include "settings.hpp"

Measurement::Measurement(uint8_t pin)
    : m_average_collector{0.0}
    , m_average_counter{0}
    , m_correction{0.0}
    , m_last_scan_value{0.0}
    , m_serialcode{0}
{
    /*
     * DS18B20 as temperature sensor
     */

    // Setup a oneWire instance to communicate with any OneWire devices
    m_onewire = new OneWire(pin);

    // Pass our oneWire reference to Dallas Temperature sensor
    m_ds18b20 = new DallasTemperature(m_onewire);

    m_ds18b20->begin();

    // get serial code of the used DS18B20
    m_ds18b20->getAddress(m_serialcode, m_device_ID);
}

Measurement::~Measurement() {}

void Measurement::meas(void)
{
    // get temperature value
    m_ds18b20->requestTemperatures();
    m_last_scan_value = m_ds18b20->getTempCByIndex(m_device_ID) + m_correction;
    m_average_collector += m_last_scan_value;
    m_average_counter++;
    DEBUG_PRINTF3("current:%f, collector:%f, counter:%d\n", m_last_scan_value, m_average_collector, m_average_counter);
}

float Measurement::getValue(void)
{
    if (m_average_collector)
        return m_average_collector / m_average_counter;
    return m_last_scan_value;
}

void Measurement::restartAverage(void)
{
    m_average_collector = 0.0;
    m_average_counter = 0;
}

void Measurement::setCorrection(const float correction)
{
    m_correction = correction;
}

const char *Measurement::getSerialCode(void)
{
    static char serial_code[SERIAL_CODE_SIZE];
    sprintf(serial_code, "%02x%02x%02x%02x%02x%02x%02x%02x"
            , m_serialcode[0], m_serialcode[1]
            , m_serialcode[2], m_serialcode[3]
            , m_serialcode[4], m_serialcode[5]
            , m_serialcode[6], m_serialcode[7]);
    return serial_code;
}

// temperature measurement via OneWire and DS18B20
Measurement g_temp_meas(ONE_WIRE_PIN);
