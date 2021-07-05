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

// Displays both colons on 1.2 inch Adafruit LED display when writing to location 2.
static const uint8_t COLONS_BITMASK = 0x02 | 0x04 | 0x08;

// Letters for 12/24 hour designation; 'A' and 'P' for 12-hour, and 'H' for 24-hour.
static const uint8_t HOUR_24_BITMASK = 0b01110110;
static const uint8_t HOUR_AM_BITMASK = 0b01110111;
static const uint8_t HOUR_PM_BITMASK = 0b01110011;

clock_display::clock_display(uint8_t time_lower_i2c_addr, uint8_t time_upper_i2c_addr, uint8_t mday_i2c_addr, uint8_t year_i2c_addr,
  uint8_t brightness, clock_mode mode)
  : cur_brightness(brightness),
    mode(mode) {
  init_led(time_lower_led, time_lower_i2c_addr);
  init_led(time_upper_led, time_upper_i2c_addr);
  init_led(mday_led, mday_i2c_addr);
  init_led(year_led, year_i2c_addr);
}

void clock_display::show_unset() {
  show_dashes(time_lower_led);
  show_dashes(time_upper_led);
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
    time_lower_led.setBrightness(brightness);
    time_upper_led.setBrightness(brightness);
    mday_led.setBrightness(brightness);
    year_led.setBrightness(brightness);
    cur_brightness = brightness;
  }
}

clock_mode clock_display::toggle_mode() {
  mode = mode == clock_12 ? clock_24 : clock_12;
  return mode;
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
  if (mode == clock_12) {
    time_upper_led.writeDigitRaw(0, time.hour < 12 ? HOUR_AM_BITMASK : HOUR_PM_BITMASK);
    uint8_t hour = time.hour % 12;
    if (hour == 0)
      hour = 12;
    if (hour < 10)
      time_upper_led.writeDigitRaw(3, 0x00);
    else
      time_upper_led.writeDigitNum(3, hour / 10 % 10);
    time_upper_led.writeDigitNum(4, hour % 10);
  } else {
    time_upper_led.writeDigitRaw(0, HOUR_24_BITMASK);
    time_upper_led.writeDigitNum(3, time.hour / 10 % 10);
    time_upper_led.writeDigitNum(4, time.hour % 10);
  }
  time_upper_led.writeDisplay();

  time_lower_led.writeDigitNum(0, time.minute / 10 % 10);
  time_lower_led.writeDigitNum(1, time.minute % 10);
  time_lower_led.writeDigitRaw(2, COLONS_BITMASK);
  time_lower_led.writeDigitNum(3, time.second / 10 % 10);
  time_lower_led.writeDigitNum(4, time.second % 10);
  time_lower_led.writeDisplay();
}
