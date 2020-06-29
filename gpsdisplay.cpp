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
#include "gpsdisplay.h"

static const uint8_t LCD_COLS = 20;
static const uint8_t LCD_ROWS = 4;

gps_display::gps_display(uint8_t i2c_addr)
  : lcd(i2c_addr - 0x70),
    searching(true) {
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();
  lcd.setBacklight(HIGH);
}

void gps_display::refresh(const gps_info& info, const gps_time& time) {
  if (searching) {
    lcd.clear();
    searching = false;
  }

  lcd.setCursor(0, 0);
  lcd.print("Satellites: ");
  lcd.print(info.satellites);

  float lat;
  char lat_dir;
  if (info.lat < 0.0) {
    lat = -info.lat;
    lat_dir = 'S';
  } else {
    lat = info.lat;
    lat_dir = 'N';
  }
  lcd.setCursor(0, 1);
  lcd.print(lat, 4);
  lcd.print(lat_dir);

  float lon;
  char lon_dir;
  if (info.lon < 0.0) {
    lon = -info.lon;
    lon_dir = 'W';
  } else {
    lon = info.lon;
    lon_dir = 'E';
  }
  lcd.setCursor(0, 2);
  lcd.print(lon, 4);
  lcd.print(lon_dir);
}

void gps_display::search() {
  if (!searching) {
    lcd.clear();
    lcd.print("Searching...");
    searching = true;
  }
}