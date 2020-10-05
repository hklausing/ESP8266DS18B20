/*-----------------------------------------------------------
NTP & time routines for ESP8266
    for ESP8266 adapted Arduino IDE
by Stefan Thesen 05/2015 - free for anyone
code for ntp adopted from Michael Margolis
code for time conversion based on http://stackoverflow.com/
-----------------------------------------------------------*/

// note: all timing relates to 1970-01-01T00:00:00Z

#include "timehelper.h"


/**
 * @brief Converts epoch time value to a ISO8601 long string
 * 
 * @param epoch - time value
 * @return String output like '2020-07-25 13:24:52'
 */
String convertEpochToIso8601(const time_t epoch)
{
    struct tm  ts;
    char buf[4+1+2+1+2+1+2+1+2+1+2+1];

    // gmtime is used to convert to localtime, because the eoch value is localtime
    ts = *gmtime(&epoch);

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &ts);
    return String(buf);
}


timer_values_t g_timer_values;