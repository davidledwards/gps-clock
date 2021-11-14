/*
 * This file was automatically generated on Sat, 13 Nov 2021 08:52:31 -0600
 */
#ifndef __CONFIG_H
#define __CONFIG_H

// Configuration for date layout.
#define DATE_LAYOUT_EU

// Configuration for LED displays that show date and time.
#define LED_LAYOUT_ROMAN
#define USE_SECONDS
#define LED_TIME_LOWER_I2C_ADDR static_cast<uint8_t>(0x71)
#define LED_TIME_UPPER_I2C_ADDR static_cast<uint8_t>(0x70)
#define LED_MDAY_I2C_ADDR static_cast<uint8_t>(0x72)
#define LED_YEAR_I2C_ADDR static_cast<uint8_t>(0x73)

// Configuration for LCD display that shows GPS information.
#define LCD_EXPANDER_PCF8574T
#define LCD_I2C_ADDR static_cast<uint8_t>(0x27)
#define LCD_GENERIC

// Configuration for light monitor that automatically sets brightness level.
#define DIMMER_PIN static_cast<uint8_t>(0)

// Configuration for 12/24-hour mode selector.
#define MODE_PIN static_cast<uint8_t>(8)
#define MODE_DEBOUNCE_MS static_cast<uint32_t>(50)

// Configuration for timezone selector.
#define TZ_A_PIN static_cast<uint8_t>(9)
#define TZ_B_PIN static_cast<uint8_t>(10)
#define TZ_BUTTON_PIN static_cast<uint8_t>(11)
#define TZ_DEBOUNCE_MS static_cast<uint32_t>(5)
#define TZ_ERROR_MS static_cast<uint32_t>(20)

// Configuration for GPS module.
#define GPS_RX_PIN static_cast<uint8_t>(3)
#define GPS_TX_PIN static_cast<uint8_t>(2)
#define GPS_BAUD_RATE static_cast<long>(9600)

// Configuration for automatically disabling LCD backlight.
#define AUTO_OFF_MS static_cast<uint32_t>(30000)

#endif
