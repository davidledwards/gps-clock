#ifndef __GPSDISPLAY_H
#define __GPSDISPLAY_H

#include <Arduino.h>
#include <Adafruit_LiquidCrystal.h>
#include "gpsunit.h"

class gps_display {
public:
  gps_display(uint8_t i2c_addr);
  void refresh(const gps_info& info, const gps_time& time);
  void search();

private:
  const Adafruit_LiquidCrystal lcd;
  bool searching;
};

#endif
