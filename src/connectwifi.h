/*
 * File         connectwifi.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-05
 * Note         Handles connecting WiFi
 */

#pragma once

typedef struct connect_wifi_state_s {
  uint32_t  reconnect;          // counts the amount of reconnected to WiFi
  bool      connected;          // is true if this system has connect to WiFi
} connect_wifi_state_t;


extern void connectWiFi();
