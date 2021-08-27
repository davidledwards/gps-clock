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
#ifndef __GPSUNIT_H
#define __GPSUNIT_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

struct gps_info {
  float lat;
  float lon;
  uint8_t satellites;
};

struct gps_time {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

enum gps_state {
  gps_searching,
  gps_available,
  gps_ignore
};

class gps_unit {
public:
  gps_unit();
  gps_state read(gps_info& info, gps_time& time);

private:
  const SoftwareSerial ser;
  const TinyGPS gps;
  uint32_t last_sync;

  static bool get_info(const TinyGPS& gps, gps_info& info);
  static bool get_time(const TinyGPS& gps, gps_time& time);
};

#endif
