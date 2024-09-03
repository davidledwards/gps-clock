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
#ifndef __STORAGE_H
#define __STORAGE_H

#include <Arduino.h>
#include "clockdisplay.h"
#include "timezones.h"

struct clock_state;

class local_state {
public:
  const char tz_name[TZ_NAME_SIZE + 1];
  clock_mode mode;

private:
  local_state(const clock_state& state);
  friend class local_storage;
};

class local_storage {
public:
  local_storage();
  local_state read();
  void write_tz(const char* tz_name);
  void write_mode(clock_mode mode);

private:
  void read_state(clock_state& state);
  void write_state(const clock_state& state);
};

#endif
