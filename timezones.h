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
#ifndef __TIMEZONES_H
#define __TIMEZONES_H

#include <Arduino.h>
#include <Timezone.h>

static const size_t TZ_NAME_SIZE = 15;

struct tz_info {
  const char* const name;
  Timezone tz;
};

class tz_database {
public:
  tz_database();
  size_t size() const;
  const tz_info* const find(const char* name) const;
  size_t find_index(const char* name) const;
  const tz_info* const get(size_t index) const;
};

#endif
