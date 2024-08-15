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
#include "gpsdisplay.h"

#if defined(GPS_DISPLAY_LCD)
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
#elif defined(GPS_DISPLAY_OLED)
// Various tiles used for single-character icons.
//
// See following link for explanation of bit layout.
// https://github.com/olikraus/u8g2/wiki/u8x8reference#drawtile
//
// Tiles are 8x8 bit arrays oriented top-to-bottom and left-to-right.

// GPS icon:
//
// ..xxx...
// .x...x..
// x.....x.
// x.....x.
// xx...xx.
// .xxxxx..
// ..xxx...
// ...x....
//
static uint8_t TILE_GPS[] = {
    0x1c, 0x32, 0x61, 0xe1, 0x61, 0x32, 0x1c, 0x00
};

// DEGREE icon
//
// ........
// ...xx...
// ..x..x..
// ..x..x..
// ...xx...
// ........
// ........
// ........
//
static uint8_t TILE_DEGREE[] = {
    0x00, 0x00, 0x0c, 0x12, 0x12, 0x0c, 0x00, 0x00
};

// ALTITUDE icon
//
// ...x....
// ..xxx...
// .xxxxx..
// ...x....
// ...x....
// .xxxxx..
// ..xxx...
// ...x....
//
static uint8_t TILE_ALTITUDE[] = {
    0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00
};

// SATELLITE icon
//
// ........
// ..xxxx..
// .x....x.
// x..xx..x
// ..x..x..
// .x....x.
// ...xx...
// ...xx...
//
static uint8_t TILE_SATELLITE[] = {
    0x08, 0x24, 0x12, 0xca, 0xca, 0x12, 0x24, 0x08
};

// CLOCK icon
//
// ........
// ..xxx...
// .x...x..
// x..x..x.
// x..xx.x.
// x.....x.
// .x...x..
// ..xxx...
//
static uint8_t TILE_CLOCK[] = {
    0x38, 0x44, 0x82, 0x9a, 0x92, 0x44, 0x38, 0x00
};

// TIMEZONE icon
//
// xxxxxxx.
// xxxxxxx.
// x.x.x.x.
// xxxxxxx.
// x.x.x.x.
// xxxxxxx.
// x.x.x.x.
// xxxxxxx.
//
static uint8_t TILE_TIMEZONE[] = {
    0xff, 0xab, 0xff, 0xab, 0xff, 0xab, 0xff, 0x00
};
#endif

gps_display::gps_display()
#if defined(GPS_DISPLAY_LCD)
  : LCD_CTOR(lcd),
#elif defined(GPS_DISPLAY_OLED)
  : oled(I2C_CLOCK_PIN, I2C_DATA_PIN),
#endif
    searching(false) {
#if defined(GPS_DISPLAY_LCD)
  LCD_INIT(lcd);
  lcd.clear();
  lcd.setBacklight(HIGH);
  lcd.createChar(CHAR_DEGREES, CHAR_DEGREES_BITMAP);
#elif defined(GPS_DISPLAY_OLED)
  oled.setI2CAddress(OLED_I2C_ADDR);
  oled.begin();
  oled.clearDisplay();
  oled.setPowerSave(0);
  oled.setFont(u8x8_font_chroma48medium8_r);
#endif
}

void gps_display::show_info(const gps_info& info, const gps_time& time) {
  if (searching) {
    clear_gps();
    searching = false;
  }
  write_lat(info);
  write_lon(info);
  write_altitude(info);
  write_satellites(info);
  write_utc(time);
}

void gps_display::show_searching() {
  if (!searching) {
    clear_gps();
#if defined(GPS_DISPLAY_LCD)
    lcd.setCursor(0, 0);
    lcd.print(F("searching..."));
#elif defined(GPS_DISPLAY_OLED)
    oled.drawTile(0, 3, 1, TILE_SATELLITE);
    oled.setCursor(2, 3);
    oled.print(F("searching..."));
#endif
    searching = true;
  }
}

void gps_display::show_tz(const tz_info* tz, bool pending) {
  write_tz(tz, pending);
}

void gps_display::show_backlight(bool on) {
#if defined(GPS_DISPLAY_LCD)
  lcd.setBacklight(on ? HIGH : LOW);
#elif defined(GPS_DISPLAY_OLED)
  oled.setPowerSave(on ? 0 : 1);
#endif
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

#if defined(GPS_DISPLAY_LCD)
  lcd.setCursor(0, 0);
  lcd.print(lat, 4);
  lcd.write(CHAR_DEGREES);
  lcd.print(lat_dir);
  if (lat < 10.0)
    lcd.print(' ');
  lcd.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  oled.drawTile(0, 0, 1, TILE_GPS);
  oled.setCursor(2, 0);
  size_t n = oled.print(lat, 6);
  oled.drawTile(n + 2, 0, 1, TILE_DEGREE);
  oled.setCursor(n + 3, 0);
  oled.print(lat_dir);
  // TODO: clear to EOL
#endif
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

#if defined(GPS_DISPLAY_LCD)
  lcd.setCursor(10, 0);
  if (lon < 100.0)
    lcd.print(' ');
  lcd.print(lon, 4);
  lcd.write(CHAR_DEGREES);
  lcd.print(lon_dir);
#elif defined(GPS_DISPLAY_OLED)
  oled.setCursor(2, 1);
  size_t n = oled.print(lon, 6);
  oled.drawTile(n + 2, 1, 1, TILE_DEGREE);
  oled.setCursor(n + 3, 1);
  oled.print(lon_dir);
  // TODO: clear to EOL
#endif
}

void gps_display::write_altitude(const gps_info& info) {
#if defined(GPS_DISPLAY_LCD)
// intentianal no-op
#elif defined(GPS_DISPLAY_OLED)
  oled.drawTile(0, 2, 1, TILE_ALTITUDE);
  oled.setCursor(2, 2);
  oled.print(static_cast<uint32_t>(info.altitude));
  oled.print(F(" m"));
  // TODO: clear to EOL
#endif
}

void gps_display::write_satellites(const gps_info& info) {
#if defined(GPS_DISPLAY_LCD)
  lcd.setCursor(0, 1);
  lcd.print(F("satellites: "));
  lcd.print(info.satellites);
  lcd.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  oled.drawTile(0, 3, 1, TILE_SATELLITE);
  oled.setCursor(2, 3);
  oled.print(info.satellites);
  // TODO: clear to EOL
#endif
}

void gps_display::write_utc(const gps_time& time) {
#if defined(GPS_DISPLAY_LCD)
  lcd.setCursor(0, 2);
#if defined(DATE_LAYOUT_ISO)
  write_year(time);
  lcd.print('-');
  write_month(time);
  lcd.print('-');
  write_day(time);
#elif defined(DATE_LAYOUT_US)
  write_month(time);
  lcd.print('-');
  write_day(time);
  lcd.print('-');
  write_year(time);
#elif defined(DATE_LAYOUT_EU)
  write_day(time);
  lcd.print('-');
  write_month(time);
  lcd.print('-');
  write_year(time);
#endif
  lcd.print(' ');
  lcd.print(time.hour / 10 % 10);
  lcd.print(time.hour % 10);
  lcd.print(':');
  lcd.print(time.minute / 10 % 10);
  lcd.print(time.minute % 10);
  lcd.print(':');
  lcd.print(time.second / 10 % 10);
  lcd.print(time.second % 10);
#elif defined(GPS_DISPLAY_OLED)
  oled.drawTile(0, 4, 1, TILE_CLOCK);
  oled.setCursor(2, 4);
#if defined(DATE_LAYOUT_ISO)
  write_year(time);
  oled.print('-');
  write_month(time);
  oled.print('-');
  write_day(time);
#elif defined(DATE_LAYOUT_US)
  write_month(time);
  oled.print('-');
  write_day(time);
  oled.print('-');
  write_year(time);
#elif defined(DATE_LAYOUT_EU)
  write_day(time);
  oled.print('-');
  write_month(time);
  oled.print('-');
  write_year(time);
#endif
  oled.setCursor(2, 5);
  oled.print(time.hour / 10 % 10);
  oled.print(time.hour % 10);
  oled.print(':');
  oled.print(time.minute / 10 % 10);
  oled.print(time.minute % 10);
  oled.print(':');
  oled.print(time.second / 10 % 10);
  oled.print(time.second % 10);
#endif
}

void gps_display::write_year(const gps_time& time) {
#if defined(GPS_DISPLAY_LCD)
  lcd.print(time.year / 1000 % 10);
  lcd.print(time.year / 100 % 10);
  lcd.print(time.year / 10 % 10);
  lcd.print(time.year % 10);
#elif defined(GPS_DISPLAY_OLED)
  oled.print(time.year / 1000 % 10);
  oled.print(time.year / 100 % 10);
  oled.print(time.year / 10 % 10);
  oled.print(time.year % 10);
#endif
}

void gps_display::write_month(const gps_time& time) {
#if defined(GPS_DISPLAY_LCD)
  lcd.print(time.month / 10 % 10);
  lcd.print(time.month % 10);
#elif defined(GPS_DISPLAY_OLED)
  oled.print(time.month / 10 % 10);
  oled.print(time.month % 10);
#endif
}

void gps_display::write_day(const gps_time& time) {
#if defined(GPS_DISPLAY_LCD)
  lcd.print(time.day / 10 % 10);
  lcd.print(time.day % 10);
#elif defined(GPS_DISPLAY_OLED)
  oled.print(time.day / 10 % 10);
  oled.print(time.day % 10);
#endif
}

void gps_display::write_tz(const tz_info* tz, bool pending) {
#if defined(GPS_DISPLAY_LCD)
  lcd.setCursor(0, 3);
  lcd.print(pending ? F("? ") : F("> "));
  size_t n = lcd.print(tz->name);
  while (n++ < 16)
    lcd.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  oled.drawTile(0, 6, 1, TILE_TIMEZONE);
  oled.setCursor(1, 6);
  oled.print(pending ? '?' : ' ');
  size_t n = oled.print(tz->name);
  // TODO: clear to EOL
#endif
}

void gps_display::clear_gps() {
  clear_row(0);
  clear_row(1);
  clear_row(2);
#if defined(GPS_DISPLAY_OLED)
  clear_row(3);
  clear_row(4);
#endif
}

void gps_display::clear_row(uint8_t row) {
#if defined(GPS_DISPLAY_LCD)
  lcd.setCursor(0, row);
  for (int i = 0; i < LCD_COLS; ++i)
    lcd.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  oled.clearLine(row);
#endif
}
