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

#include "config.h"

// Include appropriate library for controlling LCD display based on type of I/O expander.
#if defined(LCD_GENERIC)
#include <LiquidCrystal_I2C.h>
#define LCD_CLASS LiquidCrystal_I2C
#elif defined(LCD_ADAFRUIT)
#include <Adafruit_LiquidCrystal.h>
#define LCD_CLASS Adafruit_LiquidCrystal
#endif

#endif
