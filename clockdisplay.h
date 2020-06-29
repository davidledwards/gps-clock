#ifndef __CLOCKDISPLAY_H
#define __CLOCKDISPLAY_H

#include <Arduino.h>
#include <Adafruit_LEDBackpack.h>

class clock_display {
public:
  clock_display(uint8_t time_i2c_addr, uint8_t mday_i2c_addr, uint8_t year_i2c_addr);

private:
  const Adafruit_7segment time_led;
  const Adafruit_7segment mday_led;
  const Adafruit_7segment year_led;
};

#endif
