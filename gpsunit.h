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
#include <Adafruit_GPS.h>

class gps_info {
public:
  gps_info();

  float lat;
  float lon;
  uint8_t satellites;

private:
  gps_info(const Adafruit_GPS& gps);
  friend class gps_unit;
};

class gps_time {
public:
  gps_time();

  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t msecond;

private:
  gps_time(const Adafruit_GPS& gps);
  friend class gps_unit;
};

enum gps_state {
  gps_searching,
  gps_available,
  gps_ignore
};

class gps_unit {
public:
  gps_unit(uint8_t tx_pin, uint8_t rx_pin, uint32_t sync_ms);
  gps_state read(gps_info& info, gps_time& time);

private:
  const SoftwareSerial ser;
  const Adafruit_GPS gps;
  const uint32_t sync_ms;
  uint32_t last_sync;
};

#endif
