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
#ifndef __DIMMER_H
#define __DIMMER_H

#include <Arduino.h>

class light_monitor {
public:
  light_monitor();
  uint8_t get_brightness();

private:
  uint8_t pin;
  uint16_t cur_reading;
  uint8_t cur_brightness;
  uint32_t last_reading_time;
  uint16_t last_reading;
  uint32_t waiting_start;

  static uint8_t to_brightness(uint16_t reading);
};

#endif
