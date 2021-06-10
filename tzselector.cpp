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

// Proposed timezone not selected within given time frame is reverted to the previously confirmed
// adjustment.
static const uint32_t IDLE_RESET_MS = 10000;

// Period of time before the encoder reports the next event, which is designed to remove noise from the
// electrical component.
static const uint32_t ENCODER_DELAY_MS = 20;

tz_selector::tz_selector(uint8_t a_pin, uint8_t b_pin, uint8_t button_pin,
  const tz_database* tz_db, const tz_info* tz)
  : encoder(a_pin, b_pin, button_pin),
    tz_db(tz_db),
    tz_confirmed(tz_db->find_index(tz->name)),
    tz_proposed(tz_confirmed),
    last_action(0) {
  encoder.setErrorDelay(ENCODER_DELAY_MS);
}

tz_action tz_selector::read() {
  if (encoder.push() == 1) {
    // Prioritize button pushes over rotation since rotation can often occur as a side effect of pressing
    // the button. This ensures that the current selection is confirmed.
    tz_confirmed = tz_proposed;
    last_action = 0;
    return tz_confirm;
  } else {
    switch (encoder.rotate()) {
      case 0:
        // Nothing to report by the encoder, but make sure if an unconfirmed change has been idle for
        // a period of time, then it gets automatically reset.
        if (last_action > 0 && millis() - last_action > IDLE_RESET_MS) {
          reset();
          return tz_reset;
        } else
          return tz_idle;
      case 1:
        // Clockwise rotation.
        last_action = millis();
        if (++tz_proposed == tz_db->size())
          tz_proposed = 0;
        return tz_propose;
      case 2:
        // Counter-clockwise rotation.
        last_action = millis();
        tz_proposed = tz_proposed > 0 ? tz_proposed - 1 : tz_db->size() - 1;
        return tz_propose;
    }
  }
}

void tz_selector::reset() {
  tz_proposed = tz_confirmed;
  last_action = 0;
}

const tz_info* const tz_selector::get_tz() {
  return tz_db->get(tz_proposed);
}
