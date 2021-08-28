/*
 * Copyright 2021 David Edwards
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
#ifndef __LCD_H
#define __LCD_H

// Supported I/O expanders for LCDs.
#if !defined(EXPANDER_PCF8574T) && !defined(EXPANDER_PCF8574AT) && !defined(EXPANDER_MCP23008)
#error "I/O expander not detected"
#endif

// Determine type and I2C address of LCD based on I/O expander.
#if defined(EXPANDER_PCF8574T)
#define LCD_I2C_ADDR 0x27
#define LCD_GENERIC
#elif defined(EXPANDER_PCF8574AT)
#define LCD_I2C_ADDR 0x3F
#define LCD_GENERIC
#elif defined(EXPANDER_MCP23008)
#define LCD_I2C_ADDR 0x73
#define LCD_ADAFRUIT
#else
#error "LCD type not detected"
#endif

// Include appropriate library for controlling LCD display based on type of I/O expander.
#if defined(LCD_GENERIC)
#include <LiquidCrystal_I2C.h>
#define LCD_CLASS LiquidCrystal_I2C
#elif defined(LCD_ADAFRUIT)
#include <Adafruit_LiquidCrystal.h>
#define LCD_CLASS Adafruit_LiquidCrystal
#endif

#endif
