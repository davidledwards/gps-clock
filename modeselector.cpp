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
#include "modeselector.h"

// Debounce period which eliminates noise when button is activated.
static const uint32_t DEBOUNCE_MS = 50;

mode_selector::mode_selector(uint8_t pin)
  : button(pin) {
  pinMode(pin, INPUT_PULLUP);
  button.setDebounceTime(DEBOUNCE_MS);
}

bool mode_selector::toggled() {
  button.loop();
  return button.isPressed();
}
