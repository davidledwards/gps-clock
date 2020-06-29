#include "clockdisplay.h"

clock_display::clock_display(uint8_t time_i2c_addr, uint8_t mday_i2c_addr, uint8_t year_i2c_addr) {
  time_led.begin(time_i2c_addr);
  time_led.setBrightness(0);
  mday_led.begin(mday_i2c_addr);
  mday_led.setBrightness(0);
  year_led.begin(year_i2c_addr);
  year_led.setBrightness(0);
}
