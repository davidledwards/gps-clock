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
#ifndef __LOCALSTORAGE_H
#define __LOCALSTORAGE_H

#include <Arduino.h>
#include "clockdisplay.h"

struct local_state {
  long tz_adjust;
  clock_mode mode;
};

class local_storage {
public:
  local_storage();
  local_state read();
  void write_tz(long tz_adjust);
  void write_mode(clock_mode mode);
};

#endif
