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
#include "tzdatabase.h"

// Array of predefined timezones where first entry must always be "UTC", thus array size is
// guaranteed to be greater than zero. It should be safe to always reference first element.
//
// Note that names assigned to timezone change rules are irrelevant, so these are initialized
// as empty strings to minimize consumption of RAM.
static const tz_info TZ_TABLE[] = {
  tz_info {
    "UTC",
    Timezone(
      TimeChangeRule {"", Last, Sun, Mar, 1, 0}
    )
  },
  tz_info {
    "CET/CEST",
    Timezone(
      TimeChangeRule {"", Last, Sun, Mar, 2, 120},
      TimeChangeRule {"", Last, Sun, Oct, 2, 60}
    )
  },
  tz_info {
    "CST/CDT",
    Timezone(
      TimeChangeRule {"", Second, Sun, Mar, 2, -300},
      TimeChangeRule {"", First, Sun, Nov, 2, -360}
    )
  },
  tz_info {
    "EET/EEST",
    Timezone(
      TimeChangeRule {"", Last, Sun, Mar, 2, 180},
      TimeChangeRule {"", Last, Sun, Oct, 2, 120}
    )
  },
  tz_info {
    "EST/EDT",
    Timezone(
      TimeChangeRule {"", Second, Sun, Mar, 2, -240},
      TimeChangeRule {"", First, Sun, Nov, 2, -300}
    )
  },
  tz_info {
    "MST/MDT",
    Timezone(
      TimeChangeRule {"", Second, Sun, Mar, 2, -360},
      TimeChangeRule {"", First, Sun, Nov, 2, -420}
    )
  },
  tz_info {
    "PST/PDT",
    Timezone(
      TimeChangeRule {"", Second, Sun, Mar, 2, -420},
      TimeChangeRule {"", First, Sun, Nov, 2, -480}
    )
  }
};

// Number of timezones.
static const size_t TZ_TABLE_SIZE = sizeof(TZ_TABLE) / sizeof(tz_info);

tz_database::tz_database() {
}

size_t tz_database::size() {
  return TZ_TABLE_SIZE;
}

const tz_info* const tz_database::find(const char* name) {
  return &TZ_TABLE[find_index(name)];
}

size_t tz_database::find_index(const char* name) {
  for (size_t i = 0; i < TZ_TABLE_SIZE; ++i) {
    if (strcmp(TZ_TABLE[i].name, name) == 0)
      return i;
  }
  return 0;
}

const tz_info* const tz_database::get(size_t index) {
  return &TZ_TABLE[index < TZ_TABLE_SIZE ? index : 0];
}
