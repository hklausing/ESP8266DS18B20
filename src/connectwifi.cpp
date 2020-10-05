/*
 * File         connectwifi.cpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-05
 * Note         Handles functions around wifi
 */

#include <ESP8266WiFi.h>

#include "connectwifi.h"
#include "macros.h"
//#include "../../MyWiFiAccess.h"
#include "parameter.hpp"
#include "settingshandler.h"

// define connect WiFi state variable
connect_wifi_state_t wifi_server_state = {0, false};


/*
 * Connect to WiFi
 */
void connectWiFi()
{

  if(wifi_server_state.connected && WiFi.status() == WL_CONNECTED){
    return;
  }

  Serial.print(F("Connecting."));
  WiFi.mode(WIFI_STA);
  WiFi.hostname(getHostname());

  // Connect to WiFi network
  Serial.print(F("\n\nConnecting to "));
  Serial.println(getSsidName());

  // start WiFi connect with timeout check:
  // - retry after few seconds
  // - wait for user input
  WiFi.begin(getSsidName(), getPassword());
  uint32 next_marker = millis();                  // print waiting marker
  uint32 restart_counter = 30000 + next_marker;   // restart after 20 sec
  while (WiFi.status() != WL_CONNECTED || next_marker == 0)
  {
// test start
    switch (g_ih.activityStatus())
    {
    case InputStatus::Active:
        // If the parameter input is called once than a reset is required to exist this loop.
        next_marker = 0;
        break;
    case InputStatus::Write:
        saveConfig();
        // reset
        ESP.restart();
        break;
    default:
        break;
    }

    if(next_marker == 0)
      continue;
// test end

    if(next_marker < millis()) {
      Serial.print(".");
      // one dot per sec
      next_marker += 1000;
    }

    if(restart_counter < millis()){
      Serial.println(F("\nERROR: connect timeout, try restart!"));
      ESP.restart();
    }
  }
  wifi_server_state.connected = true;
  wifi_server_state.reconnect++;
  Serial.println(F("\nWiFi connected - OK"));

  // Print the IP address
  DEBUG_PRINTF1("IP address : %s\n", WiFi.localIP().toString().c_str());
  DEBUG_PRINTF1("MAC address: %s\n", WiFi.macAddress().c_str());
  DEBUG_PRINTF1("Hostname   : %s\n", WiFi.hostname().c_str());

  // // Start the server
  // server.begin();
  // Serial.println("HTTP server started - OK");
}
