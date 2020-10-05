/*
 * File         localtime.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-03
 * Note         Class declaration for local time.
 *              This class handles the followig task for the localtime:
 *              - Connect to a list of NTP servers and get a UTC time.
 *              - After the first NTP server answered other servers
 *                will be ignored.
 *              - Convert the time into linux epoch time (1.1.1970 0:0:0)
 *              - Sets the system timer
 *              - Lets the user define a time zone
 *              - Reading of the method localNow() will delivert the
 *                local time if the time zone was defined. Otherwise
 *                the UTC time will be returned.
 *              - The method updateTimer() has to be called cyclically
 *                to keep the system time in sync
 *              - Definition of NTP servers has to be done outside of
 *                the class.
 * Credits      Thanks to David Lettier (http://www.lettier.com/) for
 *              'Let's make a NTP Client in C'
 * Required Projects
 *              - Time by Michael Margolis
 *              - Timezone by Jack Christensen
 * Usage example
 *               ...
 *               #include "localtime.h"
 *               ...
 *               const char* ntp_server_list[] = {
 *                  "ptbtime1.ptb.de",
 *                  "ptbtime2.ptb.de",
 *               };
 *               ...
 *               Localtime lt(ntp_server_list, sizeof(ntp_server_list) / sizeof(ntp_server_list[0]));
 *               ...
 *               void setup()
 *               {
 *                  ...
 *                  WiFi.mode(...);
 *                  WiFi.begin(...);
 *                  waitForConnect();
 *                  ...
 *                  lt.setTimeZone(
 *                      (TimeChangeRule){ "CEST", Last, Sun, Mar, 2, 120 }, // Central European Summer Time
 *                      (TimeChangeRule){ "CET",  Last, Sun, Oct, 3, 60  }  // Central European Standard Time
 *                  );
 *                  ...
 *               }
 *               void loop()
 *               {
 *                   ...
 *                   lt.updateTimer();
 *                   g_timer.now = lt.localNow();
 *                   Serial.printf("local time: %4d-%02d-%02d %02d:%02d:%02d (%u)\n"
 *                                  , year(g_timer.now)
 *                                  , month(g_timer.now)
 *                                  , day(g_timer.now)
 *                                  , hour(g_timer.now)
 *                                  , minute(g_timer.now)
 *                                  , second(g_timer.now)
 *                                  , (unsigned int)g_timer.now);
 *                  }
 *                  ...
 *               }
 */
#pragma once

#include "ESP8266WiFi.h"
#include <WiFiUdp.h>
#include <TimeLib.h>  //by Paul Stoffregen, not included in the Arduino IDE !!!
#include <Timezone.h> //by Jack Christensen, not included in the Arduino IDE !!!


class Localtime
{
private:
    enum tz_status_t {
        TZ_NOT_DEFINED,
        TZ_ONLY_STANDARD,
        TZ_CHANGE_TIMES
    };
    /* data */
    const char** m_servers;         // array of NTP server names
    uint8_t  m_server_size;         // amount of known NTP servers
    time_t m_next_sync;             // used for time synchonizoius
    bool m_updated;                 // a NTP server has delivered the time
    tz_status_t m_timezone_defined; // local time zone must be defined before it can be used
    TimeChangeRule m_dst_time;      // struct with daylight saving time definitions
    TimeChangeRule m_std_time;      // struct with standard time definitions
    WiFiUDP m_udp;                  // udp communication for NTP

    static const int m_ntp_port = 123;          // NTP port address
    static const uint8_t m_packet_size = 48;    // NTP time is in the first 48 bytes of message

    typedef struct
    {
        uint8_t li_vn_mode;         // Eight bits. li, vn, and mode.
                                    // li.   Two bits.   Leap indicator.
                                    // vn.   Three bits. Version number of the protocol.
                                    // mode. Three bits. Client will pick mode 3 for client.
        uint8_t stratum;            // Eight bits. Stratum level of the local clock.
        uint8_t poll;               // Eight bits. Maximum interval between successive messages.
        uint8_t precision;          // Eight bits. Precision of the local clock.
        uint32_t rootDelay;         // 32 bits. Total round trip delay time.
        uint32_t rootDispersion;    // 32 bits. Max error aloud from primary clock source.
        uint32_t refId;             // 32 bits. Reference clock identifier.
        uint32_t refTm_s;           // 32 bits. Reference time-stamp seconds.
        uint32_t refTm_f;           // 32 bits. Reference time-stamp fraction of a second.
        uint32_t origTm_s;          // 32 bits. Originate time-stamp seconds.
        uint32_t origTm_f;          // 32 bits. Originate time-stamp fraction of a second.
        uint32_t rxTm_s;            // 32 bits. Received time-stamp seconds.
        uint32_t rxTm_f;            // 32 bits. Received time-stamp fraction of a second.
        uint32_t txTm_s;            // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
        uint32_t txTm_f;            // 32 bits. Transmit time-stamp fraction of a second.
    }  ntp_packet_t;
    ntp_packet_t m_packet;          //

    /*
     * Get a time from a NTP server
     */
    bool getNtpTime(int ntpServerNameId);


    /*
     * handles sending of packet data
     */
    bool sendNtpPacket(IPAddress &ntp_server_address);


public:
    Localtime() = delete;
    Localtime(const char* servers[], const int size);

    ~Localtime();

    /*
     * Defines a time zone that has no daylight saving time.
     */
    void setTimeZone(TimeChangeRule standard_time);

    /*
     * Defines a time zone that has a daylight saving time.
     */
    void setTimeZone(TimeChangeRule summer_time, TimeChangeRule standard_time);

    /*
     * Update the system timer by getting the current date/time from a
     * NTP server
     */
    bool updateTimer();

    /*
     * If local time is activated that epoch value of local time will be returned
     */
    time_t localNow();

    /*
     * If this method returns true, everthing is ok
     * if false, try a restart
     */
    bool status(void);
};
