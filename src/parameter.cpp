/*
 * File         src/parameter.cpp
 * Author       Heiko Klausing (h dot klausing at gmx do de)
 * Created      2020-09-21
 * Description  Handling of used project parameter like SSID, host name, ...
 */

#include "EEPROM.h"

#include "settings.hpp"

#include "parameter.hpp"
#include "settingshandler.h"

//
#include "../../MyWiFiAccess.h" // a dummy is defined in "MyWiFiAccess.h"

// working parameter_list data
ParameterList_t parameter_list;

String getSsidName(void)
{
    String value = parameter_list.ssid;
    return value;
}

String getPassword(void)
{
    String value = parameter_list.password;
    return value;
}

String getHostname(void)
{
    String value = parameter_list.hostname;
    return value;
}

String getLocation(void)
{
    String value = parameter_list.location;
    return value;
}

float getTempCorrection(void)
{
    float value = parameter_list.temp_correction;
    return value;
}

bool isEepromListValid(void)
{
    return (
        // Parameter version must be fit to this project version
        parameter_list.version == PARAMETER_LIST_VERSION
        // check also the block size
        && parameter_list.block_size == PARAMETER_BUFFER_SIZE
        // SSID must be set with a real name
        && parameter_list.ssid[0] != 0);
}

// void eraseConfig(void)
// {
//     // Reset EEPROM bytes to '0' for the length of the data structure
//     EEPROM.begin(PARAMETER_BUFFER_SIZE);
//     for (u_long i = eeprom_startAddress; i < PARAMETER_BUFFER_SIZE; i++)
//     {
//         EEPROM.write(i, 0);
//     }
//     delay(200);
//     EEPROM.commit();
//     EEPROM.end();
// }

void saveConfig(void)
{
    logParameterList("save parameter list");

    // Save configuration from RAM into EEPROM
    EEPROM.begin(PARAMETER_BUFFER_SIZE);
    EEPROM.put(eeprom_startAddress, parameter_list);
    // delay(200);
    EEPROM.commit(); // Only needed for ESP8266 to get data written
    EEPROM.end();    // Free RAM copy of structure
}

void loadConfig(void)
{
    // Loads configuration from EEPROM into RAM
    //SettingsData_t *sets = nullptr;
    EEPROM.begin(PARAMETER_BUFFER_SIZE);
    EEPROM.get(eeprom_startAddress, parameter_list);
    EEPROM.end();
}

bool initializeParameterList(void)
{
    Serial.println(F("parameter initialize function started"));
    bool rc = false;

    // initialize fill parameter list
    memset(&parameter_list, 0, PARAMETER_BUFFER_SIZE);
    //logParameterList("after initialize");
    g_ih.clean();
    // set here project related values
    g_ih.addSettingItem("SSID name", InputType::IT_STRING, parameter_list.ssid);
    g_ih.addSettingItem("SSID password", InputType::IT_STRING, parameter_list.password);
    g_ih.addSettingItem("Hostname", InputType::IT_STRING, parameter_list.hostname);
    g_ih.addSettingItem("Location", InputType::IT_STRING, parameter_list.location);
    g_ih.addSettingItem("Temp. correction", InputType::IT_FLOAT, &parameter_list.temp_correction);
    //logParameterList("after EEPROM copy");

    // load parameter list with data from EEPROM
    loadConfig();

    if(! isEepromListValid())
    {
        // set default values
        // set here project related values
        parameter_list.version = PARAMETER_LIST_VERSION;
        parameter_list.block_size = PARAMETER_BUFFER_SIZE;
        strcpy(parameter_list.ssid, "ssid");
        strcpy(parameter_list.password, "password");
        strcpy(parameter_list.hostname, "hostname");
        strcpy(parameter_list.location, "location");
        parameter_list.temp_correction = 0.0;
        // inform user about next step
        Serial.println(F("*** Parameter values must be renewed, press 's' to insert required values! ***"));
        //logParameterList("after setting of default values");
    }
    else
    {
        rc = true;
    }

    return rc;
}

/**
 * @brief Debug information of the current content of parameter list
 * 
 */
void logParameterList(const char * info)
{
    Serial.print(F("Content of parameter list: "));
    Serial.println(info);
    Serial.printf("  Version: %i  (exp. %i)\n", parameter_list.version, PARAMETER_LIST_VERSION);
    Serial.printf("  SSID name: '%s'\n", parameter_list.ssid);
    Serial.printf("  SSID password: '%s'\n", parameter_list.password);
    Serial.printf("  Hostname: '%s'\n", parameter_list.hostname);
    Serial.printf("  Location: '%s'\n", parameter_list.location);
    Serial.printf("  temp.Correction: %f\n", parameter_list.temp_correction);
    Serial.printf("  Size: %i (exp. %i)\n", parameter_list.block_size, PARAMETER_BUFFER_SIZE);
    Serial.println();
}