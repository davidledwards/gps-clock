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

// Define number of rows and columns for selected display.
#if defined(GPS_DISPLAY_LCD)
static const uint8_t DISPLAY_COLS = 20;
static const uint8_t DISPLAY_ROWS = 4;
#elif defined(GPS_DISPLAY_OLED)
static const uint8_t DISPLAY_COLS = 16;
#if defined(OLED_SIZE_LARGE)
static const uint8_t DISPLAY_ROWS = 8;
#elif defined(OLED_SIZE_SMALL)
static const uint8_t DISPLAY_ROWS = 4;
#endif
#endif

// Various bitmaps (LCD) and tiles (OLED) used for single-character icons.
//
// See following link for explanation of bit layout.
// https://github.com/olikraus/u8g2/wiki/u8x8reference#drawtile
//
// Bitmaps on LCD are 8x8 bit arrays oriented left-to-right and top-to-bottom.
//
// Tiles on OLED are 8x8 bit arrays oriented top-to-bottom and left-to-right.
#if defined(GPS_DISPLAY_LCD)
// DEGREE bitmap
//
// .xx..
// x..x.
// x..x.
// .xx..
// .....
// .....
// .....
// .....
//
static uint8_t BITMAP_DEGREE[] = {
  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
static const uint8_t CHAR_DEGREE = 0;

// ALTITUDE bitmap
//
// ..x..
// .xxx.
// xxxxx
// ..x..
// ..x..
// xxxxx
// .xxx.
// ..x..
//
static uint8_t BITMAP_ALTITUDE[] = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100
};
static const uint8_t CHAR_ALTITUDE = 1;

// SATELLITE bitmap
//
// .xxx.
// x...x
// ..x..
// .x.x.
// .....
// ..x..
// ..x..
// ..x..
//
static uint8_t BITMAP_SATELLITE[] = {
  0b01110,
  0b10001,
  0b00100,
  0b01010,
  0b00000,
  0b00100,
  0b00100,
  0b00100
};
static const uint8_t CHAR_SATELLITE = 2;

// TIMEZONE bitmap
//
// xxxxx
// xxxxx
// x.x.x
// xxxxx
// x.x.x
// xxxxx
// x.x.x
// xxxxx
//
static uint8_t BITMAP_TIMEZONE[] = {
  0b11111,
  0b11111,
  0b10101,
  0b11111,
  0b10101,
  0b11111,
  0b10101,
  0b11111
};
static const uint8_t CHAR_TIMEZONE = 3;
#elif defined(GPS_DISPLAY_OLED)
// GPS icon
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

// Row and column numbers of various display elements.
#if defined(GPS_DISPLAY_LCD)
// LCD has 20x4 display area.
static const uint8_t ROW_LATITUDE = 0;
static const uint8_t COL_LATITUDE = 0;
static const uint8_t ROW_LONGITUDE = 0;
static const uint8_t COL_LONGITUDE = 10;
static const uint8_t ROW_ALTITUDE = 1;
static const uint8_t COL_ALTITUDE = 0;
static const uint8_t ROW_SATELLITE = 1;
static const uint8_t COL_SATELLITE = 13;
static const uint8_t ROW_UTC = 2;
static const uint8_t COL_UTC = 0;
static const uint8_t ROW_TZ = 3;
static const uint8_t COL_TZ = 0;
static const uint8_t KEEP_ROWS = 1;
#elif defined(GPS_DISPLAY_OLED)
#if defined(OLED_SIZE_LARGE)
// OLED large has 16x8 effective display area.
static const uint8_t ROW_LATITUDE = 0;
static const uint8_t COL_LATITUDE = 0;
static const uint8_t ROW_LONGITUDE = 1;
static const uint8_t COL_LONGITUDE = 2;
static const uint8_t ROW_ALTITUDE = 2;
static const uint8_t COL_ALTITUDE = 0;
static const uint8_t ROW_SATELLITE = 3;
static const uint8_t COL_SATELLITE = 0;
static const uint8_t ROW_UTC = 4;
static const uint8_t COL_UTC = 0;
static const uint8_t ROW_TZ = 6;
static const uint8_t COL_TZ = 0;
static const uint8_t KEEP_ROWS = 2;
#elif defined(OLED_SIZE_SMALL)
// OLED small has 16x4 effective display area.
static const uint8_t ROW_LATITUDE = 0;
static const uint8_t COL_LATITUDE = 0;
static const uint8_t ROW_LONGITUDE = 1;
static const uint8_t COL_LONGITUDE = 2;
static const uint8_t ROW_ALTITUDE = 2;
static const uint8_t COL_ALTITUDE = 0;
static const uint8_t ROW_SATELLITE = 2;
static const uint8_t COL_SATELLITE = 12;
static const uint8_t ROW_UTC = 4;
static const uint8_t ROW_TZ = 3;
static const uint8_t COL_TZ = 0;
static const uint8_t KEEP_ROWS = 1;
#endif
#endif

// Decide whether or not UTC is shown based on display size constraints.
#if defined(GPS_DISPLAY_OLED) && defined(OLED_SIZE_SMALL)
#undef SHOW_UTC
#else
#define SHOW_UTC
#endif

// Unit of measure when showing altitude.
//
// GPS reports altitude in meters, hence need for corresponding divisor.
#if defined(MEASUREMENT_SYSTEM_IMPERIAL)
#define ALTITUDE_UNIT F(" ft")
static const float ALTITUDE_UNIT_DIVISOR = 0.3048;
#elif defined(MEASUREMENT_SYSTEM_METRIC)
#define ALTITUDE_UNIT F(" m")
static const float ALTITUDE_UNIT_DIVISOR = 1.0;
#endif

// Library-specific constructor and initialization code for LCD displays.
#if defined(GPS_DISPLAY_LCD)
#if defined(LCD_GENERIC)
#define LCD_CTOR(display) display(LCD_I2C_ADDR, DISPLAY_COLS, DISPLAY_ROWS)
#define LCD_INIT(display) display.init()
#elif defined(LCD_ADAFRUIT)
#define LCD_CTOR(display) display(LCD_I2C_ADDR - 0x70)
#define LCD_INIT(display) display.begin(DISPLAY_COLS, DISPLAY_ROWS)
#endif
#endif

gps_display::gps_display()
#if defined(GPS_DISPLAY_LCD)
  : LCD_CTOR(display),
#elif defined(GPS_DISPLAY_OLED)
  : display(I2C_CLOCK_PIN, I2C_DATA_PIN),
#endif
    searching(false) {
#if defined(GPS_DISPLAY_LCD)
  LCD_INIT(display);
  display.clear();
  display.setBacklight(HIGH);
  display.createChar(CHAR_DEGREE, BITMAP_DEGREE);
  display.createChar(CHAR_ALTITUDE, BITMAP_ALTITUDE);
  display.createChar(CHAR_SATELLITE, BITMAP_SATELLITE);
  display.createChar(CHAR_TIMEZONE, BITMAP_TIMEZONE);
#elif defined(GPS_DISPLAY_OLED)
  display.setI2CAddress(OLED_I2C_ADDR);
  display.begin();
  display.setPowerSave(0);
  display.clearDisplay();
  display.setFont(u8x8_font_chroma48medium8_r);
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
    display.setCursor(0, 0);
    display.write(CHAR_SATELLITE);
#elif defined(GPS_DISPLAY_OLED)
    display.drawTile(0, 0, 1, TILE_SATELLITE);
#endif

    display.setCursor(2, 0);
    display.print(F("searching..."));
    searching = true;
  }
}

void gps_display::show_tz(const tz_info* tz, bool pending) {
  write_tz(tz, pending);
}

void gps_display::show_backlight(bool on) {
#if defined(GPS_DISPLAY_LCD)
  display.setBacklight(on ? HIGH : LOW);
#elif defined(GPS_DISPLAY_OLED)
  display.setPowerSave(on ? 0 : 1);
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
  display.setCursor(COL_LATITUDE, ROW_LATITUDE);
  size_t n = COL_LATITUDE + display.print(lat, 4);
  display.write(CHAR_DEGREE);
  display.print(lat_dir);
  for (n += 2; n < 10; ++n)
    display.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(COL_LATITUDE, ROW_LATITUDE, 1, TILE_GPS);
  display.setCursor(COL_LATITUDE + 2, ROW_LATITUDE);
  size_t n = COL_LATITUDE + 2 + display.print(lat, 6);
  display.drawTile(n, ROW_LATITUDE, 1, TILE_DEGREE);
  display.setCursor(n + 1, ROW_LATITUDE);
  display.print(lat_dir);
  clear_row(ROW_LATITUDE, n + 2);
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
  display.setCursor(COL_LONGITUDE, ROW_LONGITUDE);
  if (lon < 100.0)
    display.print(' ');
  display.print(lon, 4);
  display.write(CHAR_DEGREE);
  display.print(lon_dir);
#elif defined(GPS_DISPLAY_OLED)
  display.setCursor(COL_LONGITUDE, ROW_LONGITUDE);
  size_t n = COL_LONGITUDE + display.print(lon, 6);
  display.drawTile(n, ROW_LONGITUDE, 1, TILE_DEGREE);
  display.setCursor(n + 1, ROW_LONGITUDE);
  display.print(lon_dir);
  clear_row(ROW_LONGITUDE, n + 2);
#endif
}

void gps_display::write_altitude(const gps_info& info) {
  float alt = info.altitude / ALTITUDE_UNIT_DIVISOR;

#if defined(GPS_DISPLAY_LCD)
  display.setCursor(COL_ALTITUDE, ROW_ALTITUDE);
  display.write(CHAR_ALTITUDE);
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(COL_ALTITUDE, ROW_ALTITUDE, 1, TILE_ALTITUDE);
#endif

  display.setCursor(COL_ALTITUDE + 2, ROW_ALTITUDE);
  size_t n = COL_ALTITUDE + 2 + display.print(static_cast<uint32_t>(alt));
  n += display.print(ALTITUDE_UNIT);

#if defined(GPS_DISPLAY_LCD)
  for (; n < 10; ++n)
    display.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  clear_row(ROW_ALTITUDE, n);
#endif
}

void gps_display::write_satellites(const gps_info& info) {
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(COL_SATELLITE, ROW_SATELLITE);
  display.write(CHAR_SATELLITE);
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(COL_SATELLITE, ROW_SATELLITE, 1, TILE_SATELLITE);
#endif

  display.setCursor(COL_SATELLITE + 2, ROW_SATELLITE);
  size_t n = COL_SATELLITE + 2 + display.print(info.satellites);
  clear_row(ROW_SATELLITE, n);
}

void gps_display::write_utc(const gps_time& time) {
#if defined(SHOW_UTC)
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(COL_UTC, ROW_UTC);
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(COL_UTC, ROW_UTC, 1, TILE_CLOCK);
  display.setCursor(COL_UTC + 2, ROW_UTC);
#endif

#if defined(DATE_LAYOUT_ISO)
  write_year(time);
  display.print('-');
  write_month(time);
  display.print('-');
  write_day(time);
#elif defined(DATE_LAYOUT_US)
  write_month(time);
  display.print('-');
  write_day(time);
  display.print('-');
  write_year(time);
#elif defined(DATE_LAYOUT_EU)
  write_day(time);
  display.print('-');
  write_month(time);
  display.print('-');
  write_year(time);
#endif

#if defined(GPS_DISPLAY_LCD)
  display.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  display.setCursor(COL_UTC + 2, ROW_UTC + 1);
#endif

  display.print(time.hour / 10 % 10);
  display.print(time.hour % 10);
  display.print(':');
  display.print(time.minute / 10 % 10);
  display.print(time.minute % 10);
  display.print(':');
  display.print(time.second / 10 % 10);
  display.print(time.second % 10);
#endif
}

void gps_display::write_year(const gps_time& time) {
  display.print(time.year / 1000 % 10);
  display.print(time.year / 100 % 10);
  display.print(time.year / 10 % 10);
  display.print(time.year % 10);
}

void gps_display::write_month(const gps_time& time) {
  display.print(time.month / 10 % 10);
  display.print(time.month % 10);
}

void gps_display::write_day(const gps_time& time) {
  display.print(time.day / 10 % 10);
  display.print(time.day % 10);
}

void gps_display::write_tz(const tz_info* tz, bool pending) {
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(COL_TZ, ROW_TZ);
  display.write(CHAR_TIMEZONE);
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(COL_TZ, ROW_TZ, 1, TILE_TIMEZONE);
  display.setCursor(COL_TZ + 1, ROW_TZ);
#endif

  display.print(pending ? '?' : ' ');
  size_t n = COL_TZ + 2 + display.print(tz->name);
  clear_row(ROW_TZ, n);
}

void gps_display::clear_gps() {
  for (uint8_t row = 0; row < DISPLAY_ROWS - KEEP_ROWS; ++row)
    clear_row(row);
}

void gps_display::clear_row(uint8_t row, uint8_t col) {
#if defined(GPS_DISPLAY_OLED)
  if (col == 0)
    display.clearLine(row);
  else {
#endif
    display.setCursor(col, row);
    for (; col < DISPLAY_COLS; ++col)
      display.print(' ');
#if defined(GPS_DISPLAY_OLED)
  }
#endif
}
