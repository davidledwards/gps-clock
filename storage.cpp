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
#include "storage.h"
#include "board.h"

#if defined(USE_EEPROM_EMULATION)
#if defined(ARDUINO_SAMD_NANO_33_IOT)
#include <FlashStorage.h>
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
#include <NanoBLEFlashPrefs.h>
#endif
#else
#include <EEPROM.h>
#endif

// Defines structure and contents of clock state stored to EEPROM or flash.
// Note that structure is byte-aligned to ensure backward compatibility with
// prior technique of reading and writing to EEPROM.
#pragma pack(1)
struct clock_state {
  uint16_t signature;
  char tz_name[TZ_NAME_SIZE + 1];
  bool time_12;
};
#pragma pack()

#if defined(USE_EEPROM_EMULATION)
#if defined(ARDUINO_SAMD_NANO_33_IOT)
FlashStorage(flash_storage, clock_state);
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
static NanoBLEFlashPrefs flash_storage;
#endif
#endif

// Signature expected at head of storage.
static const uint16_t SIGNATURE = 0x0001;

static char* safe_copy(char* dest, const char* src, size_t count) {
  strncpy(dest, src, count - 1);
  dest[count - 1] = '\0';
  return dest;
}

local_state::local_state(const clock_state& state)
  : tz_name { '\0' },
    mode(state.time_12 ? clock_12 : clock_24) {
  safe_copy(const_cast<char*>(this->tz_name), state.tz_name, sizeof(this->tz_name));
}

local_storage::local_storage() {
  clock_state state;
  read_state(state);

  // Initialize storage if signature not recognized.
  if (state.signature != SIGNATURE) {
    state.signature = SIGNATURE;
    strcpy(state.tz_name, "UTC");
    state.time_12 = false;
    write_state(state);
  }
}

local_state local_storage::read() {
  clock_state state;
  read_state(state);
  return local_state(state);
}

void local_storage::write_tz(const char* tz_name) {
  clock_state state;
  read_state(state);
  safe_copy(state.tz_name, tz_name, sizeof(state.tz_name));
  write_state(state);
}

void local_storage::write_mode(clock_mode mode) {
  clock_state state;
  read_state(state);
  state.time_12 = mode == clock_12;
  write_state(state);
}

void local_storage::read_state(clock_state& state) {
#if defined(USE_EEPROM_EMULATION)
#if defined(ARDUINO_SAMD_NANO_33_IOT)
  state = flash_storage.read();
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
  flash_storage.readPrefs(&state, sizeof(state));
#endif
#else
  EEPROM.get(0, state);
#endif
}

void local_storage::write_state(const clock_state& state) {
#if defined(USE_EEPROM_EMULATION)
#if defined(ARDUINO_SAMD_NANO_33_IOT)
  flash_storage.write(state);
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
  flash_storage.writePrefs(const_cast<clock_state*>(&state), sizeof(state));
#endif
#else
  EEPROM.put(0, state);
#endif
}