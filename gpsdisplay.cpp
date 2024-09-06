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
static const uint8_t DISPLAY_COLS = 21;
static const uint8_t DISPLAY_WIDTH = 128;

#if defined(OLED_SIZE_LARGE)
static const uint8_t DISPLAY_ROWS = 8;
static const uint8_t DISPLAY_HEIGHT = 64;
#elif defined(OLED_SIZE_SMALL)
static const uint8_t DISPLAY_ROWS = 4;
static const uint8_t DISPLAY_HEIGHT = 32;
#endif

// Pixel multipliers for logical col/row indexing.
static const uint8_t COL_PIXELS = 6;
static const uint8_t ROW_PIXELS = 8;
#endif

// Various bitmaps (LCD) and tiles (OLED) used for single-character icons.
//
// LCD bitmaps employ hack that uses first byte as character code, which is
// necessary for registratation with LCD libraries. This allows draw_bitmap()
// method to be used for both LCD and OLED displays.
//
// Bitmaps are oriented left-to-right and top-to-bottom.
// - LCD bitmaps are 5x8.
// - OLED bitmaps are 8x8.
//
// Visual of following bitmaps show OLED (8x8) version, not LCD (5x8) version,
// though reading bits in initializer code should give some indication as to
// layout.

// DEGREE bitmap
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
static uint8_t BITMAP_DEGREE[] = {
#if defined(GPS_DISPLAY_LCD)
  0,       // <-- character code
  0b01100, // <-- start of bitmap
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
#elif defined(GPS_DISPLAY_OLED)
  0b00000000,
  0b00110000,
  0b01001000,
  0b01001000,
  0b00110000,
  0b00000000,
  0b00000000,
  0b00000000
#endif
};

// ALTITUDE bitmap
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
static uint8_t BITMAP_ALTITUDE[] = {
#if defined(GPS_DISPLAY_LCD)
  1,       // <-- character code
  0b00100, // <-- start of bitmap
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100
#elif defined(GPS_DISPLAY_OLED)
  0b00010000,
  0b00111000,
  0b01111100,
  0b00010000,
  0b00010000,
  0b01111100,
  0b00111000,
  0b00010000
#endif
};

// SATELLITE bitmap
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
static uint8_t BITMAP_SATELLITE[] = {
#if defined(GPS_DISPLAY_LCD)
  2,       // <-- character code
  0b01110, // <-- start of bitmap
  0b10001,
  0b00100,
  0b01010,
  0b00000,
  0b00100,
  0b00100,
  0b00100
#elif defined(GPS_DISPLAY_OLED)
  0b00000000,
  0b00111100,
  0b01000010,
  0b10011001,
  0b00100100,
  0b01000010,
  0b00011000,
  0b00011000
#endif
};

// TIMEZONE bitmap
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
static uint8_t BITMAP_TIMEZONE[] = {
#if defined(GPS_DISPLAY_LCD)
  3,       // <-- character code
  0b11111, // <-- start of bitmap
  0b11111,
  0b10101,
  0b11111,
  0b10101,
  0b11111,
  0b10101,
  0b11111
#elif defined(GPS_DISPLAY_OLED)
  0b11111110,
  0b11111110,
  0b10101010,
  0b11111110,
  0b10101010,
  0b11111110,
  0b10101010,
  0b11111110
#endif
};

#if defined(GPS_DISPLAY_OLED)
// GPS bitmap
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
static uint8_t BITMAP_GPS[] = {
  0b00111000,
  0b01000100,
  0b10000010,
  0b10000010,
  0b11000110,
  0b01111100,
  0b00111000,
  0b00010000
};

// CLOCK bitmap
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
static uint8_t BITMAP_CLOCK[] = {
  0b00000000,
  0b00111000,
  0b01000100,
  0b10010010,
  0b10011010,
  0b10000010,
  0b01000100,
  0b00111000
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

gps_display::gps_display()
#if defined(GPS_DISPLAY_LCD)
#if defined(LCD_GENERIC)
  : display(LCD_I2C_ADDR),
#elif defined(LCD_ADAFRUIT)
  : display(LCD_I2C_ADDR - 0x70),
#endif
#elif defined(GPS_DISPLAY_OLED)
  : display(DISPLAY_WIDTH, DISPLAY_HEIGHT),
#endif
    searching(false),
    displaying(true) {
#if defined(GPS_DISPLAY_LCD)
  display.begin(DISPLAY_COLS, DISPLAY_ROWS);
  display.clear();
  display.createChar(BITMAP_DEGREE[0], &BITMAP_DEGREE[1]);
  display.createChar(BITMAP_ALTITUDE[0], &BITMAP_ALTITUDE[1]);
  display.createChar(BITMAP_SATELLITE[0], &BITMAP_SATELLITE[1]);
  display.createChar(BITMAP_TIMEZONE[0], &BITMAP_TIMEZONE[1]);
  display.setBacklight(HIGH);
#elif defined(GPS_DISPLAY_OLED)
  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
#endif
  display.display();
}

void gps_display::show_info(const gps_info& info, const gps_time& time) {
  if (displaying) {
    if (searching) {
      clear_gps();
      searching = false;
    }
    write_lat(info);
    write_lon(info);
    write_altitude(info);
    write_satellites(info);
    write_utc(time);
    display.display();
  }
}

void gps_display::show_searching() {
  if (displaying) {
    if (!searching) {
      clear_gps();
      draw_bitmap(0, 0, BITMAP_SATELLITE);
      set_cursor(2, 0);
      display.print(F("searching..."));
      display.display();
      searching = true;
    }
  }
}

void gps_display::show_tz(const tz_info* tz, bool pending) {
  if (displaying) {
    write_tz(tz, pending);
    display.display();
  }
}

void gps_display::show_display(bool on) {
  displaying = on;
#if defined(GPS_DISPLAY_LCD)
  if (displaying) {
    display.setBacklight(HIGH);
    display.display();
  } else {
    display.setBacklight(LOW);
    display.noDisplay();
  }
#elif defined(GPS_DISPLAY_OLED)
  display.ssd1306_command(displaying ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
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
  set_cursor(COL_LATITUDE, ROW_LATITUDE);
  size_t n = COL_LATITUDE + display.print(lat, 4);
  draw_bitmap(n, ROW_LATITUDE, BITMAP_DEGREE);
  set_cursor(n + 1, ROW_LATITUDE);
  display.print(lat_dir);
  for (n += 2; n < 10; ++n)
    display.print(' ');
#elif defined(GPS_DISPLAY_OLED)
  draw_bitmap(COL_LATITUDE, ROW_LATITUDE, BITMAP_GPS);
  set_cursor(COL_LATITUDE + 2, ROW_LATITUDE);
  size_t n = COL_LATITUDE + 2 + display.print(lat, 6);
  draw_bitmap(n, ROW_LATITUDE, BITMAP_DEGREE);
  set_cursor(n + 1, ROW_LATITUDE);
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
  set_cursor(COL_LONGITUDE, ROW_LONGITUDE);
  size_t n = COL_LONGITUDE;
  if (lon < 100.0) {
    display.print(' ');
    n += 1;
  }
  n += display.print(lon, 4);
  draw_bitmap(n, ROW_LONGITUDE, BITMAP_DEGREE);
  set_cursor(n + 1, ROW_LONGITUDE);
  display.print(lon_dir);
#elif defined(GPS_DISPLAY_OLED)
  set_cursor(COL_LONGITUDE, ROW_LONGITUDE);
  size_t n = COL_LONGITUDE + display.print(lon, 6);
  draw_bitmap(n, ROW_LONGITUDE, BITMAP_DEGREE);
  set_cursor(n + 1, ROW_LONGITUDE);
  display.print(lon_dir);
  clear_row(ROW_LONGITUDE, n + 2);
#endif
}

void gps_display::write_altitude(const gps_info& info) {
  float alt = info.altitude / ALTITUDE_UNIT_DIVISOR;

  draw_bitmap(COL_ALTITUDE, ROW_ALTITUDE, BITMAP_ALTITUDE);
  set_cursor(COL_ALTITUDE + 2, ROW_ALTITUDE);
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
  draw_bitmap(COL_SATELLITE, ROW_SATELLITE, BITMAP_SATELLITE);
  set_cursor(COL_SATELLITE + 2, ROW_SATELLITE);
  size_t n = COL_SATELLITE + 2 + display.print(info.satellites);
  clear_row(ROW_SATELLITE, n);
}

void gps_display::write_utc(const gps_time& time) {
#if defined(SHOW_UTC)
#if defined(GPS_DISPLAY_LCD)
  set_cursor(COL_UTC, ROW_UTC);
#elif defined(GPS_DISPLAY_OLED)
  draw_bitmap(COL_UTC, ROW_UTC, BITMAP_CLOCK);
  set_cursor(COL_UTC + 2, ROW_UTC);
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
  set_cursor(COL_UTC + 2, ROW_UTC + 1);
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
  draw_bitmap(COL_TZ, ROW_TZ, BITMAP_TIMEZONE);
  set_cursor(COL_TZ + 2, ROW_TZ);
  size_t n = COL_TZ + 2 + display.print(tz->name);
  display.print(pending ? '?' : ' ');
  clear_row(ROW_TZ, n + 1);
}

void gps_display::clear_gps() {
  for (uint8_t row = 0; row < DISPLAY_ROWS - KEEP_ROWS; ++row)
    clear_row(row);
}

void gps_display::clear_row(uint8_t row, uint8_t col) {
    set_cursor(col, row);
    for (; col < DISPLAY_COLS; ++col)
      display.print(' ');
}

void gps_display::set_cursor(uint8_t col, uint8_t row) {
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(col, row);
#elif defined(GPS_DISPLAY_OLED)
  display.setCursor(col * COL_PIXELS, row * ROW_PIXELS);
#endif
}

void gps_display::draw_bitmap(uint8_t col, uint8_t row, uint8_t* bitmap) {
#if defined(GPS_DISPLAY_LCD)
  display.setCursor(col, row);
  // Character code in first byte of bitmap is used as character.
  display.write(bitmap[0]);
#elif defined(GPS_DISPLAY_OLED)
  display.drawBitmap(col * COL_PIXELS, row * ROW_PIXELS, bitmap, 8, 8, WHITE);
#endif
}
