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
#include "gps.h"
#include "config.h"

// Number of milliseconds that must elapse before returning new GPS information.
static const uint32_t SYNC_DELAY_MS = 2000;

// Number of milliseconds since the last synchronization before this class starts reporting that
// the GPS module is searching.
static const uint32_t SEARCHING_DELAY_MS = 30000;

#if defined(USE_SOFTWARE_SERIAL)
#define SERIAL ser
#else
#define SERIAL Serial1
#endif

gps_unit::gps_unit()
  : last_sync(0)
#if defined(USE_SOFTWARE_SERIAL)
    , ser(GPS_TX_PIN, GPS_RX_PIN)
#endif
{
  SERIAL.begin(GPS_BAUD_RATE);
}

gps_state gps_unit::read(gps_info& info, gps_time& time) {
  while (SERIAL.available()) {
    if (gps.encode(SERIAL.read()) && millis() - last_sync > SYNC_DELAY_MS) {
      if (get_info(gps, info) && get_time(gps, time)) {
        last_sync = millis();
        return gps_available;
      }
    }
  }
  return millis() - last_sync > SEARCHING_DELAY_MS ? gps_searching : gps_ignore;
}

bool gps_unit::get_info(TinyGPSPlus& gps, gps_info& info) {
  if (gps.location.isValid() && gps.satellites.isValid() && gps.altitude.isValid()) {
    info = gps_info {
      static_cast<float>(gps.location.lat()),
      static_cast<float>(gps.location.lng()),
      static_cast<float>(gps.altitude.meters()),
      static_cast<uint8_t>(gps.satellites.value())
    };
    return true;
  } else {
    return false;
  }
}

bool gps_unit::get_time(TinyGPSPlus& gps, gps_time& time) {
  if (gps.date.isValid() && gps.time.isValid()) {
    time = gps_time {
      gps.date.year(), gps.date.month(), gps.date.day(),
      gps.time.hour(), gps.time.minute(), gps.time.second()
    };
    return true;
  } else {
    return false;
  }
}
