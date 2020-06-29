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
#include "gpsconfig.h"
#include "gpsunit.h"
#include "gpsdisplay.h"
#include "clockdisplay.h"

gps_unit* gps;
gps_display* display;
clock_display* clock;

void setup() {
  gps = new gps_unit(GPS_TX_PIN, GPS_RX_PIN, GPS_SYNC_MILLIS);
  display = new gps_display(GPS_I2C_ADDR);
  display->search();
  clock = new clock_display(CLOCK_TIME_I2C_ADDR, CLOCK_MDAY_I2C_ADDR, CLOCK_YEAR_I2C_ADDR);
}

void loop() {
  gps_info info;
  gps_time time;

  switch (gps->read(info, time)) {
    case gps_available:
      display->refresh(info, time);
      break;
    case gps_searching:
      display->search();
  }
}
