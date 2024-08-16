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
#ifndef __OLED_H
#define __OLED_H

#include "config.h"

#if defined(OLED_DRIVER_SSD1309)
#include <U8x8lib.h>
#define OLED_CLASS U8X8_SSD1309_128X64_NONAME0_SW_I2C
#endif

#endif