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
#include "gpsunit.h"

gps_info::gps_info()
  : satellites(0),
    lat(0.0),
    lon(0.0) {
}

gps_info::gps_info(const Adafruit_GPS& gps)
  : satellites(gps.satellites),
    lat(gps.latitudeDegrees),
    lon(gps.longitudeDegrees) {
}

gps_time::gps_time()
  : year(0),
    month(0),
    day(0),
    hour(0),
    minute(0),
    second(0),
    msecond(0) {
}

gps_time::gps_time(const Adafruit_GPS& gps)
  : year(gps.year),
    month(gps.month),
    day(gps.day),
    hour(gps.hour),
    minute(gps.minute),
    second(gps.seconds),
    msecond(gps.milliseconds) {
 }

gps_unit::gps_unit(uint8_t tx_pin, uint8_t rx_pin, uint32_t sync_ms)
  : ser(tx_pin, rx_pin),
    gps(&ser),
    sync_ms(sync_ms),
    last_sync(millis()) {
  // Baud rate for communicating with GPS module.
  gps.begin(9600);

  // Requests minimum amount of data including fix information.
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Ideal update rate to ensure parser works properly.
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  // Gets satellite information.
  gps.sendCommand(PGCMD_ANTENNA);
}

gps_state gps_unit::read(gps_info& info, gps_time& time) {
  gps.read();
  if (gps.newNMEAreceived() && !gps.parse(gps.lastNMEA()))
    return gps_ignore;
  else {
    uint32_t now = millis();
    if (now - last_sync > sync_ms) {
      last_sync = now;
      if (gps.fix) {
        info = gps_info(gps);
        time = gps_time(gps);
        return gps_available;
      } else
        return gps_searching;
    } else
      return gps_ignore;
  }
}
