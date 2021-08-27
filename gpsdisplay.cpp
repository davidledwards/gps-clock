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

// Library-specific constructor and initialization code.
#if defined(LCD_GENERIC)
#define LCD_CTOR(lcd) lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS)
#define LCD_INIT(lcd) lcd.init()
#elif defined(LCD_ADAFRUIT)
#define LCD_CTOR(lcd) lcd(LCD_I2C_ADDR - 0x70)
#define LCD_INIT(lcd) lcd.begin(LCD_COLS, LCD_ROWS)
#endif

gps_display::gps_display()
  : LCD_CTOR(lcd),
    searching(false) {
  LCD_INIT(lcd);
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
    lcd.print(F("searching..."));
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
    lcd.print(' ');
  lcd.print(' ');
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
    lcd.print(' ');
  lcd.print(lon, 4);
  lcd.write(CHAR_DEGREES);
  lcd.print(lon_dir);
}

void gps_display::write_satellites(const gps_info& info) {
  lcd.setCursor(0, 1);
  lcd.print(F("satellites: "));
  lcd.print(info.satellites);
  lcd.print(' ');
}

void gps_display::write_utc(const gps_time& time) {
  lcd.setCursor(0, 2);
  lcd.print(time.year / 1000 % 10);
  lcd.print(time.year / 100 % 10);
  lcd.print(time.year / 10 % 10);
  lcd.print(time.year % 10);
  lcd.print('-');
  lcd.print(time.month / 10 % 10);
  lcd.print(time.month % 10);
  lcd.print('-');
  lcd.print(time.day / 10 % 10);
  lcd.print(time.day % 10);
  lcd.print(' ');
  lcd.print(time.hour / 10 % 10);
  lcd.print(time.hour % 10);
  lcd.print(':');
  lcd.print(time.minute / 10 % 10);
  lcd.print(time.minute % 10);
  lcd.print(F(" UTC"));
}

void gps_display::write_tz(const tz_info* tz, bool pending) {
  lcd.setCursor(0, 3);
  lcd.print(F("tz"));
  if (pending)
    lcd.print(F("? "));
  else
    lcd.print(F(": "));
  size_t n = lcd.print(tz->name);
  while (n++ < 16)
    lcd.print(' ');
}

void gps_display::clear_gps() {
  clear_row(0);
  clear_row(1);
  clear_row(2);
}

void gps_display::clear_row(uint8_t row) {
  lcd.setCursor(0, row);
  for (int i = 0; i < LCD_COLS; ++i)
    lcd.print(' ');
}
