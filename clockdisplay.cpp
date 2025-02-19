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
#include "clockdisplay.h"

// Use dash on LED when time has not yet been synchronized with GPS.
static const uint8_t DASH_BITMASK = 0b01000000;

// Assigns relative position of digits on LED displays.
static const uint8_t DIGIT_0 = 0;
static const uint8_t DIGIT_1 = 1;
#if defined(LED_LAYOUT_NORMAL)
static const uint8_t DIGIT_2 = 3;
static const uint8_t DIGIT_3 = 4;
#elif defined(LED_LAYOUT_ROMAN)
static const uint8_t DIGIT_2 = 2;
static const uint8_t DIGIT_3 = 3;
#endif

// Letters for 12/24 hour designation; 'A' and 'P' for 12-hour, and 'H' for 24-hour.
#if defined(USE_SECONDS)
static const uint8_t HOUR_24_BITMASK = 0b01110110;
static const uint8_t HOUR_AM_BITMASK = 0b01110111;
static const uint8_t HOUR_PM_BITMASK = 0b01110011;
#endif

// Use dots instead of colons if specified.
#if defined(USE_DOTS)
static const bool ENABLE_DOT = true;
static const uint8_t COLON_BITMASK = 0x00;
#else
static const bool ENABLE_DOT = false;
#if defined(USE_SECONDS)
static const uint8_t COLON_BITMASK = 0x02 | 0x04 | 0x08;
#endif
#endif

clock_display::clock_display(uint8_t brightness, clock_mode mode)
  : cur_brightness(brightness),
    mode(mode),
    am_pin(AM_PIN),
    pm_pin(PM_PIN) {
#if defined(USE_SECONDS)
  init_led(time_lower_led, LED_TIME_LOWER_I2C_ADDR);
  init_led(time_upper_led, LED_TIME_UPPER_I2C_ADDR);
#else
  init_led(time_led, LED_TIME_I2C_ADDR);
#endif
  init_led(mday_led, LED_MDAY_I2C_ADDR);
  init_led(year_led, LED_YEAR_I2C_ADDR);

  // Turn off AM/PM until clock is synchronized.
  pinMode(am_pin, OUTPUT);
  pinMode(pm_pin, OUTPUT);
  digitalWrite(am_pin, LOW);
  digitalWrite(pm_pin, LOW);
}

void clock_display::show_unset() {
#if defined(USE_SECONDS)
  show_dashes(time_lower_led);
  show_dashes(time_upper_led);
#else
  show_dashes(time_led);
#endif
  show_dashes(mday_led);
  show_dashes(year_led);

  digitalWrite(am_pin, LOW);
  digitalWrite(pm_pin, LOW);
}

void clock_display::show_now(const local_time& time) {
  show_year(time);
  show_mday(time);
  show_time(time);
  show_indicator(time);
}

void clock_display::set_brightness(uint8_t brightness) {
  if (brightness != cur_brightness) {
#if defined(USE_SECONDS)
    time_lower_led.setBrightness(brightness);
    time_upper_led.setBrightness(brightness);
#else
    time_led.setBrightness(brightness);
#endif
    mday_led.setBrightness(brightness);
    year_led.setBrightness(brightness);
    cur_brightness = brightness;
  }
}

clock_mode clock_display::toggle_mode() {
  mode = mode == clock_12 ? clock_24 : clock_12;
  return mode;
}

void clock_display::init_led(Adafruit_7segment& led, uint8_t i2c_addr) {
  led.begin(i2c_addr);
  led.setBrightness(cur_brightness);
  led.clear();
  led.writeDisplay();
}

void clock_display::show_dashes(Adafruit_7segment& led) {
  led.clear();
  led.writeDigitRaw(DIGIT_0, DASH_BITMASK);
  led.writeDigitRaw(DIGIT_1, DASH_BITMASK);
  led.writeDigitRaw(DIGIT_2, DASH_BITMASK);
  led.writeDigitRaw(DIGIT_3, DASH_BITMASK);
  led.writeDisplay();
}

void clock_display::show_year(const local_time& time) {
  year_led.writeDigitNum(DIGIT_0, time.year / 1000 % 10);
  year_led.writeDigitNum(DIGIT_1, time.year / 100 % 10);
  year_led.writeDigitNum(DIGIT_2, time.year / 10 % 10);
#if defined(DATE_LAYOUT_ISO)
  // ISO layout is [YYYY.][MM.DD]
  year_led.writeDigitNum(DIGIT_3, time.year % 10, true);
#elif defined(DATE_LAYOUT_US) || defined(DATE_LAYOUT_EU)
  // US layout is [MM.DD.][YYYY]
  // EU layout is [DD.MM.][YYYY]
  year_led.writeDigitNum(DIGIT_3, time.year % 10);
#endif
  year_led.writeDisplay();
}

void clock_display::show_mday(const local_time& time) {
#if defined(DATE_LAYOUT_ISO)
  // ISO layout is [YYYY.][MM.DD]
  mday_led.writeDigitNum(DIGIT_0, time.month / 10 % 10);
  mday_led.writeDigitNum(DIGIT_1, time.month % 10, true);
  mday_led.writeDigitNum(DIGIT_2, time.day / 10 % 10);
  mday_led.writeDigitNum(DIGIT_3, time.day % 10);
#elif defined(DATE_LAYOUT_US)
  // US layout is [MM.DD.][YYYY]
  mday_led.writeDigitNum(DIGIT_0, time.month / 10 % 10);
  mday_led.writeDigitNum(DIGIT_1, time.month % 10, true);
  mday_led.writeDigitNum(DIGIT_2, time.day / 10 % 10);
  mday_led.writeDigitNum(DIGIT_3, time.day % 10, true);
#elif defined(DATE_LAYOUT_EU)
  // EU layout is [DD.MM.][YYYY]
  mday_led.writeDigitNum(DIGIT_0, time.day / 10 % 10);
  mday_led.writeDigitNum(DIGIT_1, time.day % 10, true);
  mday_led.writeDigitNum(DIGIT_2, time.month / 10 % 10);
  mday_led.writeDigitNum(DIGIT_3, time.month % 10, true);
#endif
  mday_led.writeDisplay();
}

void clock_display::show_time(const local_time& time) {
#if defined(USE_SECONDS)
  // Layout:
  //   NORMAL is
  //     with colons (default)
  //       [* HH][:MM:SS]
  //     with dots
  //       [* HH.][MM.SS]
  //   ROMAN is
  //     [HH][MM.][SS]
  //
  // where * is A/P/H to indicate AM/PM/24,
  // where . is shown only in 12-hour mode when time is PM.
#if defined(LED_LAYOUT_NORMAL)
  if (mode == clock_12) {
    time_upper_led.writeDigitRaw(DIGIT_0, time.hour < 12 ? HOUR_AM_BITMASK : HOUR_PM_BITMASK);
    uint8_t hour = time.hour % 12;
    if (hour == 0)
      hour = 12;
    if (hour < 10)
      time_upper_led.writeDigitRaw(DIGIT_2, 0x00);
    else
      time_upper_led.writeDigitNum(DIGIT_2, hour / 10 % 10);
    time_upper_led.writeDigitNum(DIGIT_3, hour % 10, ENABLE_DOT);
  } else {
    time_upper_led.writeDigitRaw(DIGIT_0, HOUR_24_BITMASK);
    time_upper_led.writeDigitNum(DIGIT_2, time.hour / 10 % 10);
    time_upper_led.writeDigitNum(DIGIT_3, time.hour % 10, ENABLE_DOT);
  }
  time_upper_led.writeDigitRaw(DIGIT_1, 0x00);
  time_upper_led.writeDisplay();

  time_lower_led.writeDigitNum(DIGIT_0, time.minute / 10 % 10);
  time_lower_led.writeDigitNum(DIGIT_1, time.minute % 10, ENABLE_DOT);
  time_lower_led.writeDigitRaw(2, COLON_BITMASK);
  time_lower_led.writeDigitNum(DIGIT_2, time.second / 10 % 10);
  time_lower_led.writeDigitNum(DIGIT_3, time.second % 10);
  time_lower_led.writeDisplay();
#else
  if (mode == clock_12) {
    uint8_t hour = time.hour % 12;
    if (hour == 0)
      hour = 12;
    if (hour < 10)
      time_upper_led.writeDigitRaw(DIGIT_0, 0x00);
    else
      time_upper_led.writeDigitNum(DIGIT_0, hour / 10 % 10);
    time_upper_led.writeDigitNum(DIGIT_1, hour % 10);
  } else {
    time_upper_led.writeDigitNum(DIGIT_0, time.hour / 10 % 10);
    time_upper_led.writeDigitNum(DIGIT_1, time.hour % 10, true);
  }
  time_upper_led.writeDigitNum(DIGIT_2, time.minute / 10 % 10);
  time_upper_led.writeDigitNum(DIGIT_3, time.minute % 10, mode == clock_12 && time.hour >= 12);
  time_upper_led.writeDisplay();

  time_lower_led.writeDigitNum(DIGIT_0, time.second / 10 % 10);
  time_lower_led.writeDigitNum(DIGIT_1, time.second % 10);
  time_lower_led.writeDisplay();
#endif
#else
  // Layout:
  //   NORMAL is
  //     with colons (default)
  //       [HH:MM.]
  //     with dots
  //       [HH.MM.]
  //   ROMAN is
  //     [HH][MM.]
  // where . is shown only in 12-hour mode when time is PM.
  if (mode == clock_12) {
    uint8_t hour = time.hour % 12;
    if (hour == 0)
      hour = 12;
    if (hour < 10)
      time_led.writeDigitRaw(DIGIT_0, 0x00);
    else
      time_led.writeDigitNum(DIGIT_0, hour / 10 % 10);
#if defined(LED_LAYOUT_NORMAL)
    time_led.writeDigitNum(DIGIT_1, hour % 10, ENABLE_DOT);
#else
    time_led.writeDigitNum(DIGIT_1, hour % 10);
#endif
  } else {
    time_led.writeDigitNum(DIGIT_0, time.hour / 10 % 10);
#if defined(LED_LAYOUT_NORMAL)
    time_led.writeDigitNum(DIGIT_1, time.hour % 10, ENABLE_DOT);
#else
    time_led.writeDigitNum(DIGIT_1, time.hour % 10);
#endif
  }
#if defined(LED_LAYOUT_NORMAL)
  time_led.drawColon(!ENABLE_DOT);
#endif
  time_led.writeDigitNum(DIGIT_2, time.minute / 10 % 10);
  time_led.writeDigitNum(DIGIT_3, time.minute % 10, mode == clock_12 && time.hour >= 12);
  time_led.writeDisplay();
#endif
}

void clock_display::show_indicator(const local_time& time) {
  if (mode == clock_12) {
    digitalWrite(am_pin, time.hour < 12 ? HIGH : LOW);
    digitalWrite(pm_pin, time.hour < 12 ? LOW : HIGH);
  } else {
    digitalWrite(am_pin, LOW);
    digitalWrite(pm_pin, LOW);
  }
}
