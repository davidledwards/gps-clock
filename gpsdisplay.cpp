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
static const uint8_t DISPLAY_ROWS = 8;
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

// Row number where latitude information is displayed.
#if defined(GPS_DISPLAY_LCD)
#define ROW_LATITUDE 0
#elif defined(GPS_DISPLAY_OLED)
#define ROW_LATITUDE 0
#endif

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
  display.setCursor(0, ROW_LATITUDE);
  size_t n = display.print(lat, 4);
  display.write(CHAR_DEGREE);
  display.print(lat_dir);
  for (n += 2; n < 10; ++n)
    display.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(0, ROW_LATITUDE, 1, TILE_GPS);
  display.setCursor(2, ROW_LATITUDE);
  size_t n = display.print(lat, 6);
  display.drawTile(n + 2, ROW_LATITUDE, 1, TILE_DEGREE);
  display.setCursor(n + 3, ROW_LATITUDE);
  display.print(lat_dir);
  clear_row(ROW_LATITUDE, n + 4);
#endif
}

// Row number where longitude information is displayed.
#if defined(GPS_DISPLAY_LCD)
#define ROW_LONGITUDE 0
#elif defined(GPS_DISPLAY_OLED)
#define ROW_LONGITUDE 1
#endif

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
  display.setCursor(10, ROW_LONGITUDE);
  if (lon < 100.0)
    display.print(' ');
  display.print(lon, 4);
  display.write(CHAR_DEGREE);
  display.print(lon_dir);
#elif defined(GPS_DISPLAY_OLED)
  display.setCursor(2, ROW_LONGITUDE);
  size_t n = display.print(lon, 6);
  display.drawTile(n + 2, ROW_LONGITUDE, 1, TILE_DEGREE);
  display.setCursor(n + 3, ROW_LONGITUDE);
  display.print(lon_dir);
  clear_row(ROW_LONGITUDE, n + 4);
#endif
}

// Row number where altitude information is displayed.
#if defined(GPS_DISPLAY_LCD)
#define ROW_ALTITUDE 1
#elif defined(GPS_DISPLAY_OLED)
#define ROW_ALTITUDE 2
#endif

void gps_display::write_altitude(const gps_info& info) {
  float alt = info.altitude / ALTITUDE_UNIT_DIVISOR;

#if defined(GPS_DISPLAY_LCD)
  display.setCursor(0, ROW_ALTITUDE);
  display.write(CHAR_ALTITUDE);
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(0, ROW_ALTITUDE, 1, TILE_ALTITUDE);
#endif

  display.setCursor(2, ROW_ALTITUDE);
  size_t n = 2 + display.print(static_cast<uint32_t>(alt));
  n += display.print(ALTITUDE_UNIT);

#if defined(GPS_DISPLAY_LCD)
  for (; n < 10; ++n)
    display.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  clear_row(ROW_ALTITUDE, n);
#endif
}

// Row number where satellite information is displayed.
#if defined(GPS_DISPLAY_LCD)
#define ROW_SATELLITE 1
#elif defined(GPS_DISPLAY_OLED)
#define ROW_SATELLITE 3
#endif

void gps_display::write_satellites(const gps_info& info) {
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(13, ROW_SATELLITE);
  display.write(CHAR_SATELLITE);
  display.setCursor(15, ROW_SATELLITE);
  size_t n = 15;
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(0, ROW_SATELLITE, 1, TILE_SATELLITE);
  display.setCursor(2, ROW_SATELLITE);
  size_t n = 2;
#endif

  n += display.print(info.satellites);
  clear_row(ROW_SATELLITE, n);
}

// Row number where UTC time information is displayed.
#if defined(GPS_DISPLAY_LCD)
#define ROW_UTC 2
#elif defined(GPS_DISPLAY_OLED)
#define ROW_UTC 4
#endif

void gps_display::write_utc(const gps_time& time) {
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(0, ROW_UTC);
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(0, ROW_UTC, 1, TILE_CLOCK);
  display.setCursor(2, ROW_UTC);
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
  display.setCursor(2, ROW_UTC + 1);
#endif

  display.print(time.hour / 10 % 10);
  display.print(time.hour % 10);
  display.print(':');
  display.print(time.minute / 10 % 10);
  display.print(time.minute % 10);
  display.print(':');
  display.print(time.second / 10 % 10);
  display.print(time.second % 10);
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

// Row number where timezone information is displayed.
#if defined(GPS_DISPLAY_LCD)
#define ROW_TZ 3
#elif defined(GPS_DISPLAY_OLED)
#define ROW_TZ 6
#endif

void gps_display::write_tz(const tz_info* tz, bool pending) {
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(0, ROW_TZ);
  display.write(CHAR_TIMEZONE);
#elif defined(GPS_DISPLAY_OLED)
  display.drawTile(0, ROW_TZ, 1, TILE_TIMEZONE);
  display.setCursor(1, ROW_TZ);
#endif

  display.print(pending ? '?' : ' ');
  size_t n = 2 + display.print(tz->name);
  clear_row(ROW_TZ, n);
}

void gps_display::clear_gps() {
#if defined(GPS_DISPLAY_LCD)
  // On LCD 20x4 display, last row is not cleared since this contains selected
  // timezone.
  for (uint8_t row = 0; row < DISPLAY_ROWS - 1; ++row)
    clear_row(row);
#elif defined(GPS_DISPLAY_OLED)
  // On OLED 16x8 display, last two rows are not cleared, as next to last shows
  // selected timezone and last shows timezone being selected, if active.
  for (uint8_t row = 0; row < DISPLAY_ROWS - 2; ++row)
    clear_row(row);
#endif
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
