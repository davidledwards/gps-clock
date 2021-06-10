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

static const uint8_t CHAR_DEGREES = 0;
static const uint8_t CHAR_DEGREES_BITMAP[] = {
  0b00001100,
  0b00010010,
  0b00010010,
  0b00001100,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};

static const char BLANK_LINE[] = "                    ";

gps_display::gps_display(uint8_t i2c_addr)
#if defined(USE_PCF8574T) || defined(USE_PCF8574AT)
  : lcd(i2c_addr, LCD_COLS, LCD_ROWS),
#elif defined(USE_MCP23008)
  : lcd(i2c_addr - 0x70),
#endif
    searching(false) {
#if defined(USE_PCF8574T) || defined(USE_PCF8574AT)
  lcd.init();
#elif defined(USE_MCP23008)
  lcd.begin(LCD_COLS, LCD_ROWS);
#endif
  lcd.clear();
  lcd.setBacklight(HIGH);
  lcd.createChar(CHAR_DEGREES, CHAR_DEGREES_BITMAP);
}

void gps_display::show_info(const gps_info& info, const gps_time& time) {
  if (searching) {
    clear_gps();
    searching = false;
  }

  write_lat(info);
  write_lon(info);
  write_satellites(info);
  write_utc(time);
}

void gps_display::show_searching() {
  if (!searching) {
    clear_gps();
    lcd.setCursor(0, 0);
    lcd.print("searching...");
    searching = true;
  }
}

void gps_display::show_tz(const tz_info* tz, bool pending) {
  write_tz(tz, pending);
}

void gps_display::show_backlight(bool on) {
  lcd.setBacklight(on ? HIGH : LOW);
}

void gps_display::write_lat(const gps_info& info) {
  float lat;
  char lat_dir;

  if (info.lat < 0.0) {
    lat = -info.lat;
    lat_dir = 'S';
  } else {
    lat = info.lat;
    lat_dir = 'N';
  }
  lcd.setCursor(0, 0);
  lcd.print(lat, 4);
  lcd.write(CHAR_DEGREES);
  lcd.print(lat_dir);
  if (lat < 10.0)
    lcd.print("  ");
  else
    lcd.print(" ");
}

void gps_display::write_lon(const gps_info& info) {
  float lon;
  char lon_dir;

  if (info.lon < 0.0) {
    lon = -info.lon;
    lon_dir = 'W';
  } else {
    lon = info.lon;
    lon_dir = 'E';
  }
  lcd.setCursor(10, 0);
  if (lon < 100.0)
    lcd.print(" ");
  lcd.print(lon, 4);
  lcd.write(CHAR_DEGREES);
  lcd.print(lon_dir);
}

void gps_display::write_satellites(const gps_info& info) {
  lcd.setCursor(0, 1);
  lcd.print("satellites: ");
  lcd.print(info.satellites);
  lcd.print(" ");
}

void gps_display::write_utc(const gps_time& time) {
  lcd.setCursor(0, 2);
  lcd.print("20");
  if (time.year < 10)
    lcd.print('0');
  lcd.print(time.year);
  lcd.print('-');
  if (time.month < 10)
    lcd.print('0');
  lcd.print(time.month);
  lcd.print('-');
  if (time.day < 10)
    lcd.print('0');
  lcd.print(time.day);
  lcd.print(' ');
  if (time.hour < 10)
    lcd.print('0');
  lcd.print(time.hour);
  lcd.print(':');
  if (time.minute < 10)
    lcd.print('0');
  lcd.print(time.minute);
  lcd.print(" UTC");
}

void gps_display::write_tz(const tz_info* tz, bool pending) {
  lcd.setCursor(0, 3);
  lcd.print("tz");
  if (pending)
    lcd.print("? ");
  else
    lcd.print(": ");
  size_t n = lcd.print(tz->name);
  while (n++ < 16)
    lcd.print(" ");
}

void gps_display::clear_gps() {
  clear_row(0);
  clear_row(1);
  clear_row(2);
}

void gps_display::clear_row(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print(BLANK_LINE);
}
