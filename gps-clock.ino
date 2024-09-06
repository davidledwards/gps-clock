/*
 * Copyright 2021 David Edwards
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
#include <Arduino.h>
#include "gps.h"
#include "gpsdisplay.h"
#include "clock.h"
#include "clockdisplay.h"
#include "timezones.h"
#include "selector.h"
#include "mode.h"
#include "storage.h"
#include "dimmer.h"
#include "config.h"

static local_storage* storage;
static gps_unit* gps;
static tz_database* tz_db;
static local_clock* lcl_clock;
static tz_selector* tz_sel;
static mode_selector* mode_sel;
static gps_display* gps_disp;
static clock_display* clock_disp;
static light_monitor* light_mon;

// Time of last TZ selector movement or 0 if LCD display is turned off.
static uint32_t last_movement;

void setup() {
  // Fetch state from persistent storage.
  storage = new local_storage();
  local_state state = storage->read();

  // Initialize GPS module.
  gps = new gps_unit();

  // Determine local timezone.
  tz_db = new tz_database();
  const tz_info* tz = tz_db->find(state.tz_name);

  // Initialize local clock with persisted timezone.
  lcl_clock = new local_clock(tz);

  // Initialize timezone selector componnent.
  tz_sel = new tz_selector(tz_db, tz);

  // Initialize 12/24 time selector.
  mode_sel = new mode_selector();

  // Initialize GPS display.
  gps_disp = new gps_display();
  gps_disp->show_tz(tz_sel->get_tz(), false);

  // Initialize photoresistor-based light monitor.
  light_mon = new light_monitor();

  // Initialize localized clock display.
  clock_disp = new clock_display(light_mon->get_brightness(), state.mode);
  clock_disp->show_unset();

  // Keep LCD backlight initially on when clock is restarted.
  last_movement = millis();
}

void loop() {
  // Read the TZ selector before making updates to the displays since it might result in a change
  // to the timezone.
  tz_action action = tz_sel->read();
  const tz_info* tz = tz_sel->get_tz();

  // Intelligently turn LCD backlight on/off.
  if (action == tz_idle) {
    if (last_movement > 0 && millis() - last_movement > AUTO_OFF_MS) {
      gps_disp->show_display(false);
      last_movement = 0;
    }
  } else if (action != tz_reset) {
    if (last_movement == 0) {
      // When backlight is turned on due to some kind of selector movement, the event is quietly ignored.
      gps_disp->show_display(true);
      tz_sel->reset();
      action = tz_idle;
    }
    last_movement = millis();
  }

  // Possibly update timezone.
  if (action != tz_idle)
    lcl_clock->set_tz(tz);
  if (action == tz_confirm)
    storage->write_tz(tz->name);

  // Read the 12/24 selector and toggle the current mode if activated, which also requires an
  // immediate refresh of the display so as not to wait until the next tick.
  if (mode_sel->toggled()) {
    clock_mode mode = clock_disp->toggle_mode();
    clock_disp->show_now(lcl_clock->now());
    storage->write_mode(mode);
  }

  // Read the GPS module, which will almost always return `gps_ignore` once a satellite fix has
  // been established.
  gps_info info;
  gps_time time;
  gps_state state = gps->read(info, time);
  switch (state) {
    case gps_available:
      // Under normal circumstances in which a fix has been established, this only happens roughly
      // every GPS_SYNC_MILLIS. This is a good time to synchronize the clock.
      gps_disp->show_info(info, time);
      lcl_clock->sync(time);
      break;
    case gps_searching:
      // Indicates that a satellite fix has not been established by the GPS module.
      gps_disp->show_searching();
      break;
  }

  // Any activity from the TZ selector is reflected on the display, e.g. rotation of the encoder
  // will be reflected in an unconfirmed change in the timezone, whereas a push of the encoder
  // commits the change.
  if (action != tz_idle)
    gps_disp->show_tz(tz, action == tz_propose);

  // If the local clock has changed since the last tick or a timezone was changed, then update
  // the display.
  if (lcl_clock->is_sync() && (lcl_clock->tick() || action != tz_idle))
    clock_disp->show_now(lcl_clock->now());

  // Change brightness level of the clock. In most cases, this results in a no-op since the light
  // monitor samples on a periodic basis and the clock will only adjust brightness if the level
  // actually changed.
  clock_disp->set_brightness(light_mon->get_brightness());
}
