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
#include "lightmonitor.h"

// Period of time before next photoresistor reading. This is necessary to avoid rapid flickering
// since photoresistor can produce range of results that would oscillate between adjacent
// brightness levels.
static const uint32_t SAMPLE_DELAY_MS = 5000;

// The range of values returned by analog pins [0-1024), which is a mapping of voltage to said range.
static const uint16_t PIN_RANGE = 1024;

// Range of analog pin values that map to a brightness level (0-15). A given pin value can be
// divided by this number to obtain the brightness level.
static const uint16_t BRIGHTNESS_RANGE = 64;

light_monitor::light_monitor(uint8_t pin)
  : pin(pin),
    cur_reading(PIN_RANGE / 2),
    cur_brightness(to_brightness(cur_reading)),
    last_sample(0) {
}

uint8_t light_monitor::get_brightness() {
  if (millis() - last_sample > SAMPLE_DELAY_MS) {
    int value = analogRead(pin);
    uint16_t reading = value < 0 ? 0 : value < PIN_RANGE ? value : PIN_RANGE;
    // Changes in brightness level are recorded only when the difference in photoresistor pin reading
    // is at least as large as the brightness range. Reasoning for this behavior is to avoid scenarios
    // where the pin value oscillates back and forth between brightness range boundaries, which would
    // then result in constant flickering in the display.
    if (abs(reading - cur_reading) >= BRIGHTNESS_RANGE) {
      cur_reading = reading;
      cur_brightness = to_brightness(reading);
    }
    last_sample = millis();
  }
  return cur_brightness;
}

uint8_t light_monitor::to_brightness(uint16_t reading) {
  return reading / BRIGHTNESS_RANGE;
}
