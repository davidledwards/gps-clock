/*
 * Copyright 2024 David Edwards
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
#ifndef __BOARD_H
#define __BOARD_H

#include <Arduino.h>
#include "config.h"

// Set directives based on board type.
//
// RAM_SIZE
//   Amount of SRAM in KB.
//
// USE_SOFTWARE_SERIAL
//   If serial interface is software-based.
//
// USE_EEPROM_EMULATION
//   If EEPROM is native or emulated.

#if defined(ARDUINO_AVR_UNO)
#define RAM_SIZE 2
#define USE_SOFTWARE_SERIAL
#undef USE_EEPROM_EMULATION
#elif defined(ARDUINO_AVR_NANO)
#define RAM_SIZE 2
#define USE_SOFTWARE_SERIAL
#undef USE_EEPROM_EMULATION
#elif defined(ARDUINO_AVR_MEGA1280) || defined(ARDUINO_AVR_MEGA2560)
#define RAM_SIZE 8
#define USE_SOFTWARE_SERIAL
#undef USE_EEPROM_EMULATION
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
#define RAM_SIZE 32
#undef USE_SOFTWARE_SERIAL
#define USE_EEPROM_EMULATION
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
#define RAM_SIZE 256
#undef USE_SOFTWARE_SERIAL
#define USE_EEPROM_EMULATION
#else
#error "ARDUINO_?: board type not supported"
#endif

#endif
