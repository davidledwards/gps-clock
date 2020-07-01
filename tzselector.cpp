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
#include "tzselector.h"

// UTC-12:00
static const long TZ_FLOOR = -43200;

// UTC+14:00
static const long TZ_CEILING = 50400;

// Adjust by 30-minute increments to provide finer-grained control.
static const long TZ_INCREMENT = 1800;

// Proposed time adjustments not selected within given time frame are reverted to the previously confirmed
// adjustment.
static const uint32_t IDLE_RESET_MS = 10000;

tz_selector::tz_selector(uint8_t a_pin, uint8_t b_pin, uint8_t button_pin)
  : encoder(a_pin, b_pin, button_pin),
    tz_confirmed(0),
    tz_proposed(0),
    last_action(0) {
  encoder.setErrorDelay(200);
}

tz_action tz_selector::read() {
  if (encoder.push() == 1) {
    tz_confirmed = tz_proposed;
    return tz_confirm;
  } else {
    switch (encoder.rotate()) {
      case 0:
        if (millis() - last_action > IDLE_RESET_MS) {
          tz_proposed = tz_confirmed;
          last_action = millis();
          return tz_propose;
        } else
          return tz_idle;
      case 1:
        last_action = millis();
        if (tz_proposed < TZ_CEILING) {
          tz_proposed += TZ_INCREMENT;
          return tz_propose;
        } else
          return tz_idle;
      case 2:
        last_action = millis();
        if (tz_propose > TZ_FLOOR) {
          tz_proposed -= TZ_INCREMENT;
          return tz_propose;
        } else
          return tz_idle;
    }
  }
}

long tz_selector::get_tz() {
  return tz_proposed;
}
