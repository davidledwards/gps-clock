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

static const tz_info TZ_TABLE[] = {
  tz_info {
    "UTC",
    Timezone(
      TimeChangeRule {"UTC", Last, Sun, Mar, 1, 0}
    )
  },
  tz_info {
    "CET/CEST",
    Timezone(
      TimeChangeRule {"CEST", Last, Sun, Mar, 2, 120},
      TimeChangeRule {"CET", Last, Sun, Oct, 2, 60}
    )
  },
  tz_info {
    "CST/CDT",
    Timezone(
      TimeChangeRule {"CDT", Second, Sun, Mar, 2, -300},
      TimeChangeRule {"CST", First, Sun, Nov, 2, -360}
    )
  },
  tz_info {
    "EET/EEST",
    Timezone(
      TimeChangeRule {"EEST", Last, Sun, Mar, 2, 180},
      TimeChangeRule {"EET", Last, Sun, Oct, 2, 120}
    )
  }
};

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
