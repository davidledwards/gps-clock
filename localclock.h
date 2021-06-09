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
#ifndef __LOCALCLOCK_H
#define __LOCALCLOCK_H

#include <Arduino.h>
#include <TimeLib.h>
#include "gpsunit.h"
#include "tzdatabase.h"

class local_time {
public:
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;

private:
  local_time(time_t t);
  friend class local_clock;
};

class local_clock {
public:
  local_clock(const tz_info* tz);
  bool tick();
  local_time now();
  void set_tz(const tz_info* tz);
  void sync(const gps_time& time);

private:
  time_t last_time;
  const tz_info* tz;
};

#endif
