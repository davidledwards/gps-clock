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
#include "gpsunit.h"

// GPS module baud rate.
static const long GPS_BAUD_RATE = 9600;

// Number of milliseconds that must elapse before returning new GPS information.
static const uint32_t SYNC_DELAY_MS = 2000;

// Number of milliseconds since the last synchronization before this class starts reporting that
// the GPS module is searching.
static const uint32_t SEARCHING_DELAY_MS = 30000;

gps_unit::gps_unit(uint8_t tx_pin, uint8_t rx_pin)
  : ser(tx_pin, rx_pin),
    last_sync(0) {
  ser.begin(GPS_BAUD_RATE);
}

gps_state gps_unit::read(gps_info& info, gps_time& time) {
  while (ser.available()) {
    if (gps.encode(ser.read()) && millis() - last_sync > SYNC_DELAY_MS) {
      if (get_info(gps, info) && get_time(gps, time)) {
        last_sync = millis();
        return gps_available;
      }
    }
  }
  return millis() - last_sync > SEARCHING_DELAY_MS ? gps_searching : gps_ignore;
}

bool gps_unit::get_info(const TinyGPS& gps, gps_info& info) {
  float lat;
  float lon;
  unsigned long age;
  gps.f_get_position(&lat, &lon, &age);
  unsigned short satellites = gps.satellites();
  if (age == TinyGPS::GPS_INVALID_AGE || satellites == TinyGPS::GPS_INVALID_SATELLITES)
    return false;
  else {
    info = gps_info {lat, lon, satellites};
    return true;
  }
}

bool gps_unit::get_time(const TinyGPS& gps, gps_time& time) {
  int year;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, 0, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    return false;
  else {
    time = gps_time {year, month, day, hour, minute, second};
    return true;
  }
}
