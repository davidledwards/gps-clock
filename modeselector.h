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
#ifndef __MODESELECTOR_H
#define __MODESELECTOR_H

#include <Arduino.h>
#include <ezButton.h>
#include "gpsconfig.h"

class mode_selector {
public:
  mode_selector(uint8_t pin);

  bool toggled();

private:
  ezButton button;
};

#endif
