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
#include <EEPROM.h>
#include "localstorage.h"

// Signature expected at head of storage.
static const uint16_t SIG = 0x0001;

static const int SIG_ADDR = 0;
static const int STATE_ADDR = SIG_ADDR + sizeof(SIG);

static char* safe_copy(char* dest, const char* src, size_t count) {
  strncpy(dest, src, count - 1);
  dest[count - 1] = '\0';
  return dest;
}

local_state::local_state(const char* tz_name, clock_mode mode)
  : mode(mode) {
  safe_copy(this->tz_name, tz_name, sizeof(this->tz_name));
}

local_storage::local_storage() {
  uint16_t sig;
  EEPROM.get(SIG_ADDR, sig);

  // Initialize storage if signature not recognized.
  if (sig != SIG) {
    EEPROM.put(SIG_ADDR, SIG);
    write_tz("UTC");
    write_mode(clock_24);
  }
}

local_state local_storage::read() {
  char tz_name[TZ_NAME_SIZE + 1];
  bool time_12;
  EEPROM.get(STATE_ADDR, tz_name);
  tz_name[sizeof(tz_name) - 1] = '\0';
  EEPROM.get(STATE_ADDR + sizeof(tz_name), time_12);
  return local_state(tz_name, time_12 ? clock_12 : clock_24);
}

void local_storage::write_tz(const char* tz_name) {
  char name[TZ_NAME_SIZE + 1];
  safe_copy(name, tz_name, sizeof(name));
  EEPROM.put(STATE_ADDR, name);
}

void local_storage::write_mode(clock_mode mode) {
  EEPROM.put(STATE_ADDR + TZ_NAME_SIZE + 1, mode == clock_12);
}
