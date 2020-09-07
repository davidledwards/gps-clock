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

// Period of time before the encoder reports the next event, which is designed to remove noise from the
// electrical component.
static const uint32_t ENCODER_DELAY_MS = 5;

tz_selector::tz_selector(uint8_t a_pin, uint8_t b_pin, uint8_t button_pin, long tz_default)
  : encoder(a_pin, b_pin, button_pin),
    tz_confirmed(sanitize_tz(tz_default)),
    tz_proposed(tz_confirmed),
    last_action(0) {
  encoder.setErrorDelay(ENCODER_DELAY_MS);
}

tz_action tz_selector::read() {
  if (encoder.push() == 1) {
    // Prioritize button pushes over rotation since rotation can often occur as a side effect of pressing
    // the button. This ensures that the current selection is confirmed.
    tz_confirmed = tz_proposed;
    return tz_confirm;
  } else {
    switch (encoder.rotate()) {
      case 0:
        // Nothing to report by the encoder, but make sure if an unconfirmed change has been idle for
        // a period of time, then it gets automatically reset.
        if (millis() - last_action > IDLE_RESET_MS) {
          tz_proposed = tz_confirmed;
          last_action = millis();
          return tz_propose;
        } else
          return tz_idle;
      case 1:
        // Clockwise rotation.
        last_action = millis();
        if (tz_proposed < TZ_CEILING)
          tz_proposed += TZ_INCREMENT;
        return tz_propose;
      case 2:
        // Counter-clockwise rotation.
        last_action = millis();
        if (tz_proposed > TZ_FLOOR)
          tz_proposed -= TZ_INCREMENT;
        return tz_propose;
    }
  }
}

long tz_selector::get_tz() {
  return tz_proposed;
}

long tz_selector::sanitize_tz(long offset) {
  offset = offset / TZ_INCREMENT * TZ_INCREMENT;
  if (offset < TZ_FLOOR)
    return TZ_FLOOR;
  else if (offset > TZ_CEILING)
    return TZ_CEILING;
  else
    return offset;
}
