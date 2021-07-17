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
#include "gpsconfig.h"
#include "gpsunit.h"
#include "localclock.h"

enum clock_mode {
  clock_12,
  clock_24
};

class clock_display {
public:
  clock_display(uint8_t time_lower_i2c_addr, uint8_t time_upper_i2c_addr, uint8_t mday_i2c_addr, uint8_t year_i2c_addr,
    uint8_t brightness, clock_mode mode);

  void show_unset();
  void show_now(const local_time& time);
  void set_brightness(uint8_t brightness);
  clock_mode toggle_mode();

private:
  const Adafruit_7segment time_lower_led;
  const Adafruit_7segment time_upper_led;
  const Adafruit_7segment mday_led;
  const Adafruit_7segment year_led;
  uint8_t cur_brightness;
  clock_mode mode;

  void init_led(const Adafruit_7segment& led, uint8_t i2c_addr);
  void show_dashes(const Adafruit_7segment& led);
  void show_year(const local_time& time);
  void show_mday(const local_time& time);
  void show_time(const local_time& time);
};

#endif
