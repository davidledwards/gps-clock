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
#include "modeselector.h"
#include "localstorage.h"
#include "lightmonitor.h"

static local_storage* storage;
static gps_unit* gps;
static local_clock* clock;
static tz_selector* tz_sel;
static mode_selector* mode_sel;
static gps_display* gps_disp;
static clock_display* clock_disp;
static light_monitor* light_mon;

void setup() {
  // Fetch state from persistent storage.
  storage = new local_storage();
  local_state state = storage->read();

  // Initialize GPS module.
  gps = new gps_unit(GPS_TX_PIN, GPS_RX_PIN, GPS_SYNC_MILLIS);

  // Initialize local clock with persisted timezone.
  clock = new local_clock();
  clock->set_tz(state.tz_adjust);

  // Initialize timezone selector componnent.
  tz_sel = new tz_selector(TZ_A_PIN, TZ_B_PIN, TZ_BUTTON_PIN, state.tz_adjust);

  // Initialize 12/24 time selector.
  mode_sel = new mode_selector(MODE_PIN);

  // Initialize GPS display.
  gps_disp = new gps_display(GPS_I2C_ADDR);
  gps_disp->show_tz(tz_sel->get_tz());

  // Initialize photoresistor-based light monitor.
  light_mon = new light_monitor(PHOTORESISTOR_PIN);

  // Initialize localized clock display.
  clock_disp = new clock_display(TIME_I2C_ADDR, MDAY_I2C_ADDR, YEAR_I2C_ADDR, light_mon->get_brightness(), state.mode);
  clock_disp->show_unset();
}

void loop() {
  // Read the TZ selector before making updates to the displays since it might result in a change
  // to the timezone.
  tz_action action = tz_sel->read();
  if (action == tz_confirm) {
    long tz_adjust = tz_sel->get_tz();
    clock->set_tz(tz_adjust);
    storage->write_tz(tz_adjust);
  }

  // Read the 12/24 selector and toggle the current mode if activated, which also requires an
  // immediate refresh of the display so as not to wait until the next tick.
  if (mode_sel->toggled()) {
    clock_mode mode = clock_disp->toggle_mode();
    clock_disp->show_now(clock->now());
    storage->write_mode(mode);
  }

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

  // Change brightness level of the clock. In most cases, this results in a no-op since the light
  // monitor samples on a periodic basis and the clock will only adjust brightness if the level
  // actually changed.
  clock_disp->set_brightness(light_mon->get_brightness());
}
