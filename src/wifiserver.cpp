/*
 * File         src/wifiserver.hpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-09-22
 * Note         Handles WiFi server activities
 */

#include "wifiserver.hpp"
#include "settings.hpp"


// Init WEB server
WiFiServer g_wifi_server(WEB_SERVER_PORT);

