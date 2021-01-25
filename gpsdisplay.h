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
#ifndef __GPSDISPLAY_H
#define __GPSDISPLAY_H

#include <Arduino.h>
#include <Adafruit_LiquidCrystal.h>
#include "gpsunit.h"

class gps_display {
public:
  gps_display(uint8_t i2c_addr);
  void show_info(const gps_info& info, const gps_time& time);
  void show_searching();
  void show_tz(long tz_adjust);
  void show_backlight(bool on);

private:
  const Adafruit_LiquidCrystal lcd;
  bool searching;

  void write_lat(const gps_info& info);
  void write_lon(const gps_info& info);
  void write_satellites(const gps_info& info);
  void write_utc(const gps_time& time);
  void write_tz(long tz_adjust);
  void clear_gps();
  void clear_row(uint8_t row);
};

#endif
