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
#include "localstorage.h"
#include <EEPROM.h>

// Signature expected at head of storage.
static const uint16_t SIG = 0xabcd;

static const int SIG_ADDR = 0;
static const int STATE_ADDR = SIG_ADDR + sizeof(SIG);

local_state::local_state(long tz_adjust)
  : tz_adjust(tz_adjust) {
}

local_storage::local_storage() {
  uint16_t sig;
  EEPROM.get(SIG_ADDR, sig);

  // Initialize storage if signature not recognized.
  if (sig != SIG) {
    EEPROM.put(SIG_ADDR, SIG);
    EEPROM.put(STATE_ADDR, (long) 0);
  }
}

local_state local_storage::read() {
  long tz_adjust;
  EEPROM.get(STATE_ADDR, tz_adjust);
  return local_state(tz_adjust);
}

void local_storage::write_tz(long tz_adjust) {
  EEPROM.put(STATE_ADDR, tz_adjust);
}
