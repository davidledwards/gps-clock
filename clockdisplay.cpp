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
#include "clockdisplay.h"

const uint8_t DASH_BITMASK = 0b01000000;

clock_display::clock_display(uint8_t time_i2c_addr, uint8_t mday_i2c_addr, uint8_t year_i2c_addr) {
  init_led(time_led, time_i2c_addr);
  init_led(mday_led, mday_i2c_addr);
  init_led(year_led, year_i2c_addr);
}

void clock_display::init_led(const Adafruit_7segment& led, uint8_t i2c_addr) {
  led.begin(i2c_addr);
  led.setBrightness(0);
  show_dashes(led);
}

void clock_display::show_dashes(const Adafruit_7segment& led) {
  led.clear();
  led.writeDigitRaw(0, DASH_BITMASK);
  led.writeDigitRaw(1, DASH_BITMASK);
  led.writeDigitRaw(3, DASH_BITMASK);
  led.writeDigitRaw(4, DASH_BITMASK);
  led.writeDisplay();
}
