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
#ifndef __BOARD_H
#define __BOARD_H

// Detect supported board type and RAM size (KB).
#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
#define BOARD_UNO
#define RAM_SIZE 2
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define BOARD_MEGA
#define RAM_SIZE 8
#else
#error "board type not supported"
#endif

#endif
