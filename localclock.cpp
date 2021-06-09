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
#include "localclock.h"

local_time::local_time(time_t t)
  : year(::year(t)),
    month(::month(t)),
    day(::day(t)),
    hour(::hour(t)),
    minute(::minute(t)) {
}

local_clock::local_clock(const tz_info* tz)
  : tz(tz),
    last_time(0) {
}

bool local_clock::tick() {
  if (timeStatus() == timeNotSet)
    return false;
  else {
    time_t cur_time = tz->tz.toLocal(::now());
    if (cur_time == last_time)
      return false;
    else {
      last_time = cur_time;
      return true;
    }
  }
}

local_time local_clock::now() {
  return local_time(last_time);
}

void local_clock::set_tz(const tz_info* tz) {
  this->tz = tz;
}

void local_clock::sync(const gps_time& time) {
  setTime(time.hour, time.minute, time.second, time.day, time.month, 2000 + time.year);
}
