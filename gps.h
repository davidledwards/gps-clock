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
#ifndef __GPS_H
#define __GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include "board.h"

#if defined(USE_SOFTWARE_SERIAL)
#include <SoftwareSerial.h>
#endif

struct gps_info {
  float lat;
  float lon;
  float altitude;
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
#if defined(USE_SOFTWARE_SERIAL)
  SoftwareSerial ser;
#endif
  TinyGPSPlus gps;
  uint32_t last_sync;

  static bool get_info(TinyGPSPlus& gps, gps_info& info);
  static bool get_time(TinyGPSPlus& gps, gps_time& time);
};

#endif
