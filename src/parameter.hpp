/*
 * File         src/parameter.hpp
 * Author       Heiko Klausing (h dot klausing at gmx do de)
 * Created      2020-09-21
 * Description  Handling of used project parameter like SSID, host name, ...
 */

#pragma once

#include <Arduino.h>
#include "settings.hpp"

/**
 * @brief parameter list of project related data
 * 
 * set here project related values
 */
typedef struct
{
    int version;                // Value != 0 defines a configuration
    char ssid[STRING_SIZE];     // SSID of WiFi
    char password[STRING_SIZE]; // passowrd for WiFi
    char hostname[STRING_SIZE]; // name of the host
    char location[STRING_SIZE]; // location of the temp-logger
    float temp_correction;      // correction value for temperature
    int block_size; // size of this data block
} ParameterList_t;

constexpr size_t PARAMETER_BUFFER_SIZE = sizeof(ParameterList_t);

/**
 * @brief Get the Ssid Name object
 * 
 * @return String& 
 */
String getSsidName(void);

/**
 * @brief Get the Password object
 * 
 * @return String& 
 */
String getPassword(void);

/**
 * @brief Get the Hostname object
 * 
 * @return String& 
 */
String getHostname(void);

/**
 * @brief Get the Location object
 * 
 * @return String& 
 */
String getLocation(void);

/**
 * @brief Get the Temp Correction object
 * 
 * @return String& 
 */
float getTempCorrection(void);

/**
 * @brief Return the status of the parameter list
 * 
 * @return true Content of the parameter list is valid and can be used
 * @return false Content of the parameter is not valied, user has to correct the content.
 */
bool isEepromListValid(void);

// /**
//  * @brief 
//  * 
//  */
// void eraseConfig(void);

/**
 * @brief 
 * 
 */
void saveConfig(void);

/**
 * @brief Load the parameter list buffer from EEPROM or with default values
 * 
 */
void loadConfig(void);


/**
 * @brief inizialize the parameter input feature
 * 
 * It is expected that this function is use in the setup() function of the sketch.
 * 
 * @return true The data in parameter list are valid für use
 * @return false The data in parmater list have to be corrected for usage
 */
bool initializeParameterList(void);


/**
 * @brief 
 * 
 * @param info 
 */
void logParameterList(const char * info="");


/// working settings data
extern ParameterList_t settings;
