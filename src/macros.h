/*
 * File         macros.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-05
 * Note         Global macros
 */

#pragma once

/*
 * Macro definitions
 */
#undef DEBUG_OUTPUT
#define DEBUG_OUTPUT

#ifdef DEBUG_OUTPUT
  #define DEBUG_PRINT(x1) Serial.print(x1)
  #define DEBUG_PRINTF1(f,x1) Serial.printf(f,x1)
  #define DEBUG_PRINTF2(f,x1,x2) Serial.printf(f,x1,x2)
  #define DEBUG_PRINTF3(f,x1,x2,x3) Serial.printf(f,x1,x2,x3)
  #define DEBUG_PRINTF4(f,x1,x2,x3,x4) Serial.printf(f,x1,x2,x3,x4)
  #define DEBUG_PRINTLN(x1) Serial.println(x1)
#else
  #define DEBUG_PRINT(x1)
  #define DEBUG_PRINTF1(f,x1)
  #define DEBUG_PRINTF2(f,x1,x2)
  #define DEBUG_PRINTF3(f,x1,x2,x3)
  #define DEBUG_PRINTF4(f,x1,x2,x3,x4)
  #define DEBUG_PRINTLN(x1)
#endif

#define LED_ON LOW
#define LED_OFF HIGH
