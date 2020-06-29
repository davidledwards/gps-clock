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
#ifndef __CLOCKDISPLAY_H
#define __CLOCKDISPLAY_H

#include <Arduino.h>
#include <Adafruit_LEDBackpack.h>

class clock_display {
public:
  clock_display(uint8_t time_i2c_addr, uint8_t mday_i2c_addr, uint8_t year_i2c_addr);

private:
  const Adafruit_7segment time_led;
  const Adafruit_7segment mday_led;
  const Adafruit_7segment year_led;

  void init_led(const Adafruit_7segment& led, uint8_t i2c_addr);
  void show_dashes(const Adafruit_7segment& led);
};

#endif
