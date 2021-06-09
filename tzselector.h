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
#ifndef __TZSELECTOR_H
#define __TZSELECTOR_H

#include <Arduino.h>
#include <SimpleRotary.h>
#include "tzdatabase.h"

enum tz_action {
  tz_idle,
  tz_propose,
  tz_confirm
};

class tz_selector {
public:
  tz_selector(uint8_t a_pin, uint8_t b_pin, uint8_t button_pin, const tz_database* tz_db, const tz_info* tz);
  tz_action read();
  const tz_info* const get_tz();

private:
  const SimpleRotary encoder;
  const tz_database* tz_db;
  size_t tz_confirmed;
  size_t tz_proposed;
  uint32_t last_action;
};

#endif
