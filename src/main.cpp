/*
 * File         src/main.cpp
 * Author       Heiko Klausing (h dot klausing at gmx do de)
 * Created      2020-08-01
 * Description  Temperature logger with web server support to get access to the
 *              result values via a web server interface
 */

#include <ESP8266mDNS.h>
#include "localtime.h"
#include "connectwifi.h"
#include "meas.h"
#include "miniringbuffer.hpp"
#include "timehelper.h"
#include "parameter.hpp"
#include "webserver.hpp"
#include "signal.hpp"
#include "measbuffer.hpp"
#include "wifiserver.hpp"
#include "settingshandler.h"
#ifdef ARDUINO_OTA_ENABLE
#include "ArduinoOTA.h"
#endif

/*
 * NTP server addreses
 */
static const char *ntp_server_list[] = {
    "ptbtime1.ptb.de",
    "ptbtime2.ptb.de",
    "ptbtime3.ptb.de",
};
Localtime g_lt(ntp_server_list, sizeof(ntp_server_list) / sizeof(ntp_server_list[0]));

// Start the WEB server and inform the user via terminal output
void startWiFiServer(void)
{
    // Start the server
    g_wifi_server.begin();
    Serial.println("WEB Server started");
}

/// parameter status; is true if parameter can be used for WiFi access
bool g_isParameterListUsable = false;

/******************************************************************************
 * 
 * setup()
 * 
 ******************************************************************************/

void setup()
{

    // terminal communication
    Serial.begin(115200);

    // activate the LEDs to give a life sign at startup
    activityLed.ledOff();
    webPageActivityLed.ledOff();
    // keep the last number at 0 to switch off all LEDs
    for (auto i : {2, 0, 2, 0, 1, 0, 1, 0, 2, 1, 2, 1, 3, 0, 3, 0})
    {
        switch (i)
        {
        case 0:
            activityLed.ledOff();
            webPageActivityLed.ledOff();
            break;
        case 1:
            activityLed.ledOn();
            webPageActivityLed.ledOff();
            break;
        case 2:
            activityLed.ledOff();
            webPageActivityLed.ledOn();
            break;
        default:
            activityLed.ledOn();
            webPageActivityLed.ledOn();
            break;
        }
        delay(250);
    }

    /*
     * get parameter values from EEPROM is possible
     *  Required data:
     *  - WiFi SSID name
     *  - WiFi SSID password
     *  - hostname for this temperature logger
     *  - location for this temperature logger
     *  - correction value for this temperature logger sensor
     */
    Serial.println("Start EEPROM read");
    g_isParameterListUsable = initializeParameterList();
    if (!g_isParameterListUsable)
    {
        Serial.println(F("Temperature Logger cannot be started, parameter missing"));
        return;
    }

    // connect to WiFi and come back if connection is established
    connectWiFi();
    startWiFiServer();

    // add hostname to DNS
    if (!MDNS.begin(getHostname()))
    {
        Serial.println(F("ERROR setting up MDNS responder!"));
    }

    g_lt.setTimeZone(
        (TimeChangeRule){"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
        (TimeChangeRule){"CET", Last, Sun, Oct, 3, 60}    // Central European Standard Time
    );

    for (int i = 0; i < 10; i++)
    {
        if (!g_lt.updateTimer())
        {
            delay(500);
            Serial.print(".");
        }
        else
        {
            break;
        }
    }
    if (!g_lt.status())
    {
        // NTP server was not found try restart
        Serial.println(F("\nERROR: NTP connect not possible, try restart!"));
        delay(5000);
        ESP.restart();
    }
    else
    {
        Serial.println(" - NTP ok");
    }

    // force immediately temperature store
    g_timer_values.next_store_temp = millis() + 35000;        // wait to get some scans for average
    g_timer_values.store_interval = MEASURMENT_DOMAIN * 1000; // [ms]
    g_timer_values.next_meas_temp = 0;
    g_timer_values.meas_interval = TIME_MEASUREMENT_DISTANCE * 1000; // [ms]
    g_timer_values.start_timestamp = g_lt.localNow();                // Temperature logger start time

    // correct temperature value
    g_temp_meas.setCorrection(getTempCorrection()); // reduce temperature by one degree

#ifdef ARDUINO_OTA_ENABLE
    // OTA
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });
    ArduinoOTA.begin();
#endif
}

/******************************************************************************
 * 
 * loop()
 * 
 ******************************************************************************/

void loop()
{
    bool disable_output = false;

    // check i f user wants to modify the parameter list
    switch (g_ih.activityStatus())
    {
    case InputStatus::Active:
        disable_output = true;
        break;
    case InputStatus::Write:
        saveConfig();
        // reset
        ESP.restart();
        break;
    default:
        disable_output = false;
        break;
    }

    if (!g_isParameterListUsable)
    {
        // stop further executions if parameter list is not usable
        return;
    }

    if (!disable_output)
    {

        // get current timer value for this loop run
        g_timer_values.now = millis();

        // temperature measurement; for average calculation
        if (g_timer_values.next_meas_temp < g_timer_values.now)
        {
            activityLed.ledOn();

            g_timer_values.next_meas_temp = g_timer_values.now + g_timer_values.meas_interval;
            //DEBUG_PRINTLN("meas");

            g_lt.updateTimer();
            MDNS.update();

            // start next measurement
            g_temp_meas.meas();

            activityLed.ledOff();
        }

        // temperature store (store to ringbuffer)
        if (g_timer_values.next_store_temp < g_timer_values.now || g_ringbuffer.size() == 0)
        {
            activityLed.ledOn();

            // set value for nect
            g_timer_values.next_store_temp = g_timer_values.now + g_timer_values.store_interval;
            g_measvalue.temperature = g_temp_meas.getValue();
            g_measvalue.timestamp = g_lt.localNow();
            g_ringbuffer.add(g_measvalue);

            Serial.printf("%s, Measured temp. : %f °C , counter:%u\n",
                          convertEpochToIso8601(g_measvalue.timestamp).c_str(),
                          g_measvalue.temperature,
                          g_ringbuffer.size());
            g_temp_meas.restartAverage();

            activityLed.ledOff();
        }
    }

    // check if WLAN is connected
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Restart WiFi!"));
        connectWiFi();
        startWiFiServer();
        g_lt.updateTimer();
        MDNS.update();
        return;
    }

    g_prj_web_server.processClient();

#ifdef ARDUINO_OTA_ENABLE
    // OTA
    ArduinoOTA.handle();
#endif
}
