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

static const uint8_t DASH_BITMASK = 0b01000000;

clock_display::clock_display(uint8_t time_i2c_addr, uint8_t mday_i2c_addr, uint8_t year_i2c_addr, uint8_t brightness)
  : cur_brightness(brightness) {
  init_led(time_led, time_i2c_addr);
  init_led(mday_led, mday_i2c_addr);
  init_led(year_led, year_i2c_addr);
}

void clock_display::show_unset() {
  show_dashes(time_led);
  show_dashes(mday_led);
  show_dashes(year_led);
}

void clock_display::show_now(const local_time& time) {
  show_year(time);
  show_mday(time);
  show_time(time);
}

void clock_display::set_brightness(uint8_t brightness) {
  if (brightness != cur_brightness) {
    time_led.setBrightness(brightness);
    mday_led.setBrightness(brightness);
    year_led.setBrightness(brightness);
    cur_brightness = brightness;
  }
}

void clock_display::init_led(const Adafruit_7segment& led, uint8_t i2c_addr) {
  led.begin(i2c_addr);
  led.setBrightness(cur_brightness);
  led.clear();
  led.writeDisplay();
}

void clock_display::show_dashes(const Adafruit_7segment& led) {
  led.clear();
  led.writeDigitRaw(0, DASH_BITMASK);
  led.writeDigitRaw(1, DASH_BITMASK);
  led.writeDigitRaw(3, DASH_BITMASK);
  led.writeDigitRaw(4, DASH_BITMASK);
  led.writeDisplay();
}

void clock_display::show_year(const local_time& time) {
  year_led.writeDigitNum(0, time.year / 1000 % 10);
  year_led.writeDigitNum(1, time.year / 100 % 10);
  year_led.writeDigitNum(3, time.year / 10 % 10);
  year_led.writeDigitNum(4, time.year % 10, true);
  year_led.writeDisplay();
}

void clock_display::show_mday(const local_time& time) {
  mday_led.writeDigitNum(0, time.month / 10 % 10);
  mday_led.writeDigitNum(1, time.month % 10, true);
  mday_led.writeDigitNum(3, time.day / 10 % 10);
  mday_led.writeDigitNum(4, time.day % 10);
  mday_led.writeDisplay();
}

void clock_display::show_time(const local_time& time) {
  time_led.writeDigitNum(0, time.hour / 10 % 10);
  time_led.writeDigitNum(1, time.hour % 10);
  time_led.drawColon(true);
  time_led.writeDigitNum(3, time.minute / 10 % 10);
  time_led.writeDigitNum(4, time.minute % 10);
  time_led.writeDisplay();
}
