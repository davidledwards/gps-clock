#ifndef __GPS_UNIT_H
#define __GPS_UNIT_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

class gps_info {
public:
  float lat;
  float lon;
  uint8_t satellites;

  gps_info();

private:
  gps_info(const Adafruit_GPS& gps);
  friend class gps_unit;
};

class gps_time {
public:
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t msecond;

  gps_time();

private:
  gps_time(const Adafruit_GPS& gps);
  friend class gps_unit;
};

enum gps_state {
  gps_searching,
  gps_available,
  gps_ignore
};

class gps_unit {
public:
  gps_unit(uint8_t tx_pin, uint8_t rx_pin, uint32_t sync_ms);
  gps_state read(gps_info& info, gps_time& time);

private:
  const SoftwareSerial ser;
  const Adafruit_GPS gps;
  const uint32_t sync_ms;
  uint32_t last_sync;
};

#endif
