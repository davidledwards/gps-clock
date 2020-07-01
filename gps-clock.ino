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
#include "localclock.h"
#include "clockdisplay.h"
#include "tzselector.h"

gps_unit* gps;
local_clock* clock;
tz_selector* tz_sel;
gps_display* gps_disp;
clock_display* clock_disp;

void setup() {
  gps = new gps_unit(GPS_TX_PIN, GPS_RX_PIN, GPS_SYNC_MILLIS);
  clock = new local_clock();
  tz_sel = new tz_selector(TZ_A_PIN, TZ_B_PIN, TZ_BUTTON_PIN);

  gps_disp = new gps_display(GPS_I2C_ADDR);
  gps_disp->show_tz(tz_sel->get_tz());

  clock_disp = new clock_display(TIME_I2C_ADDR, MDAY_I2C_ADDR, YEAR_I2C_ADDR);
  clock_disp->show_unset();
}

void loop() {
  tz_action action = tz_sel->read();
  if (action == tz_confirm)
    clock->set_tz(tz_sel->get_tz());

  gps_info info;
  gps_time time;
  switch (gps->read(info, time)) {
    case gps_available:
      gps_disp->show_info(info, time);
      clock->sync(time);
      break;
    case gps_searching:
      gps_disp->show_searching();
      break;
  }

  if (action != tz_idle)
    gps_disp->show_tz(tz_sel->get_tz());

  if (clock->tick())
    clock_disp->show_now(clock->now());
}
