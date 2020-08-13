/*
 * Copyright 2020 David Edwards
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __GPSCONFIG_H
#define __GPSCONFIG_H

// Pins attached to the GPS module.
const uint8_t GPS_RX_PIN = 7;
const uint8_t GPS_TX_PIN = 8;

// Number of milliseconds before reading the GPS.
const uint32_t GPS_SYNC_MILLIS = 2000;

// I2C address of the 20x4 LCD display used to show GPS information.
#if defined(USE_PCF8574T)
const uint8_t GPS_I2C_ADDR = 0x27;
#elif defined(USE_PCF8574AT)
const uint8_t GPS_I2C_ADDR = 0x3F;
#elif defined(USE_MCP23008)
const uint8_t GPS_I2C_ADDR = 0x73;
#else
#error define one of: USE_PCF8574T, USE_PCF8574AT, USE_MCP23008
#endif

// I2C addresses of the various 4-digit LED displays used to show local time.
const uint8_t TIME_I2C_ADDR = 0x70;
const uint8_t MDAY_I2C_ADDR = 0x71;
const uint8_t YEAR_I2C_ADDR = 0x72;

// Pins attached to the rotary encoder used for selecting the timezone offset.
const uint8_t TZ_A_PIN = 9;
const uint8_t TZ_B_PIN = 10;
const uint8_t TZ_BUTTON_PIN = 11;

// Analog pin connected to photoresistor for light monitor.
const uint8_t PHOTORESISTOR_PIN = 0;

#endif
