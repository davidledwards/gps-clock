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

static gps_unit* gps;
static local_clock* clock;
static tz_selector* tz_sel;
static gps_display* gps_disp;
static clock_display* clock_disp;

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
  // Read the TZ selector before making updates to the displays since it might result in a change
  // to the timezone.
  tz_action action = tz_sel->read();
  if (action == tz_confirm)
    clock->set_tz(tz_sel->get_tz());

  // Read the GPS module, which will almost always return `gps_ignore` once a satellite fix has
  // been established.
  gps_info info;
  gps_time time;
  switch (gps->read(info, time)) {
    case gps_available:
      // Under normal circumstances in which a fix has been established, this only happens roughly
      // every GPS_SYNC_MILLIS. This is a good time to synchronize the clock.
      gps_disp->show_info(info, time);
      clock->sync(time);
      break;
    case gps_searching:
      // Indicates that a satellite fix has not been established by the GPS module.
      gps_disp->show_searching();
      break;
  }

  // Any activity from the TZ selector is reflected on the display, e.g. rotation of the encoder
  // will be reflected in an unconfirmed change in the TZ offset, whereas a push of the encoder
  // commits the change.
  if (action != tz_idle)
    gps_disp->show_tz(tz_sel->get_tz());

  // If the local clock has changed since the last tick, then update the display.
  if (clock->tick())
    clock_disp->show_now(clock->now());
}
