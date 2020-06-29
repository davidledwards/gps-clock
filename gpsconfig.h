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
#ifndef __GPSCONFIG_H
#define __GPSCONFIG_H

const uint8_t GPS_RX_PIN = 7;
const uint8_t GPS_TX_PIN = 8;
const uint32_t GPS_SYNC_MILLIS = 5000;

const uint8_t GPS_I2C_ADDR = 0x73;

const uint8_t CLOCK_TIME_I2C_ADDR = 0x70;
const uint8_t CLOCK_MDAY_I2C_ADDR = 0x71;
const uint8_t CLOCK_YEAR_I2C_ADDR = 0x72;

#endif
