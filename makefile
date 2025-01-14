#
# Copyright 2021 David Edwards
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
ifneq (,$(wildcard ./.config))
	include .config
	export
endif

SKETCH = gps-clock
BOARD_PACKAGE = arduino
BOARD_ARCH ?= avr
BOARD ?= nano
BOARD_CORE = $(BOARD_PACKAGE):$(BOARD_ARCH)
BOARD_NAME = $(BOARD_CORE):$(BOARD)

# This variable specifies the serial port for uploading bits to the board and must be defined by
# the environment, otherwise the value defaults to /dev/null.
PORT ?= /dev/null

# A convenient technique for deterministically placing the build directoy somewhere other than the
# project directory since this is not allowed.
BUILD_ROOT = $(abspath $(TMPDIR)/build)
BUILD_PATH = $(abspath $(BUILD_ROOT)/$(subst /,_,$(CURDIR)))
BUILD_FILES_PREFIX = $(SKETCH).$(subst :,.,$(BOARD_NAME))

# Library dependencies.
LIBS = \
	"Adafruit LED Backpack Library"@1.5.1 \
	"Adafruit LiquidCrystal"@2.0.4 \
	"LiquidCrystal_PCF8574"@2.2.0 \
	"Adafruit SSD1306"@2.5.13 \
	"TinyGPSPlus"@1.0.3 \
	"SimpleRotary"@1.1.3 \
	"Time"@1.6.1 \
	"Timezone"@1.2.4 \
	"ezButton"@1.0.6 \
	"FlashStorage"@1.0.0 \
	"NanoBLEFlashPrefs"@1.2.0

# Name of the image that gets uploaded to the board.
PROG = $(BUILD_FILES_PREFIX).hex

# Sources for compilation.
SRCS = $(wildcard *.ino *.h *.cpp)

# Configuration sources and targets
#
# Any file matching ".config*" will have a corresponding "config*.h" file
# generated when `make config` is executed.
CONFIG_SRCS = $(wildcard configs/.config*)
CONFIG_TARGETS = $(patsubst configs/.%,configs/%.h,$(CONFIG_SRCS))

# This section defines configuration variables which are used to generate
# configuration header files. In most cases, variables are assigned default
# values if undefined.

# Configuration for date layout.
CONFIG_DATE_LAYOUT ?= ISO

# System of measurement.
CONFIG_MEASUREMENT_SYSTEM ?= IMPERIAL

# Configuration for LED displays that show date and time.
CONFIG_LED_LAYOUT ?= NORMAL

ifdef CONFIG_USE_SECONDS
CONFIG_LED_TIME_LOWER_I2C_ADDR ?= 0x70
CONFIG_LED_TIME_UPPER_I2C_ADDR ?= 0x71
CONFIG_LED_MDAY_I2C_ADDR ?= 0x72
CONFIG_LED_YEAR_I2C_ADDR ?= 0x73
else
CONFIG_LED_TIME_I2C_ADDR ?= 0x70
CONFIG_LED_MDAY_I2C_ADDR ?= 0x71
CONFIG_LED_YEAR_I2C_ADDR ?= 0x72
endif

# Configuration for AM/PM pins.
CONFIG_AM_PIN ?= 4
CONFIG_PM_PIN ?= 5

# Configuration for GPS display that shows GPS information.
CONFIG_GPS_DISPLAY ?= LCD

ifeq ($(CONFIG_GPS_DISPLAY), LCD)
# Configuration for LCD 20x4 display that shows GPS information.
CONFIG_LCD_DRIVER ?= PCF8574T

ifeq ($(CONFIG_LCD_DRIVER), PCF8574T)
CONFIG_LCD_I2C_ADDR ?= 0x27
CONFIG_LCD_TYPE ?= GENERIC
else ifeq ($(CONFIG_LCD_DRIVER), PCF8574AT)
CONFIG_LCD_I2C_ADDR ?= 0x3F
CONFIG_LCD_TYPE ?= GENERIC
else ifeq ($(CONFIG_LCD_DRIVER), MCP23008)
CONFIG_LCD_I2C_ADDR ?= 0x73
CONFIG_LCD_TYPE ?= ADAFRUIT
endif
else ifeq ($(CONFIG_GPS_DISPLAY), OLED)
# Configuration for OLED display.
CONFIG_OLED_I2C_ADDR_DEFAULT = 0x3C
CONFIG_OLED_I2C_ADDR ?= $(CONFIG_OLED_I2C_ADDR_DEFAULT)

# Configuration for OLED display size.
CONFIG_OLED_SIZE ?= LARGE
endif

# Configuration for light monitor that automatically sets brightness level.
CONFIG_DIMMER_PIN ?= 0

# Configuration for 12/24-hour mode selector.
CONFIG_MODE_PIN ?= 2
CONFIG_MODE_DEBOUNCE_MS ?= 50

# Configuration for timezone selector.
CONFIG_TZ_A_PIN ?= 9
CONFIG_TZ_B_PIN ?= 10
CONFIG_TZ_BUTTON_PIN ?= 11
CONFIG_TZ_DEBOUNCE_MS ?= 5
CONFIG_TZ_ERROR_MS ?= 20

# Configuration for GPS module.
ifeq ($(BOARD), uno)
CONFIG_GPS_RX_PIN_DEFAULT = 7
CONFIG_GPS_TX_PIN_DEFAULT = 8
else ifeq ($(BOARD), nano)
CONFIG_GPS_RX_PIN_DEFAULT = 7
CONFIG_GPS_TX_PIN_DEFAULT = 8
else ifeq ($(BOARD), mega)
CONFIG_GPS_RX_PIN_DEFAULT = 50
CONFIG_GPS_TX_PIN_DEFAULT = 51
else ifeq ($(BOARD), nano_33_iot)
CONFIG_GPS_RX_PIN_DEFAULT = 0
CONFIG_GPS_TX_PIN_DEFAULT = 1
else ifeq ($(BOARD), nano33ble)
CONFIG_GPS_RX_PIN_DEFAULT = 0
CONFIG_GPS_TX_PIN_DEFAULT = 1
else ifeq ($(BOARD), nona4809)
CONFIG_GPS_RX_PIN_DEFAULT = 0
CONFIG_GPS_TX_PIN_DEFAULT = 1
endif

CONFIG_GPS_RX_PIN ?= $(CONFIG_GPS_RX_PIN_DEFAULT)
CONFIG_GPS_TX_PIN ?= $(CONFIG_GPS_TX_PIN_DEFAULT)
CONFIG_GPS_BAUD_RATE ?= 9600

# Configuration for automatically disabling LCD backlight.
CONFIG_AUTO_OFF_MS ?= 30000

.PHONY: help install build upload clean config print

help:
	@echo "useful targets:"
	@echo "  install   install library and core dependencies"
	@echo "  build     build sketch"
	@echo "  upload    upload program to board"
	@echo "  clean     remove all transient build files"
	@echo "  config    generate predefined configuration files"
	@echo "  print     print configuration variables"

$(PROG): $(SRCS)
	@echo "building..."
	arduino-cli compile \
		--build-path $(BUILD_PATH) \
		-b $(BOARD_NAME)

build: $(PROG)

upload: build
	@echo "uploading to ${PORT}..."
	arduino-cli upload \
	  --input-dir $(BUILD_PATH) \
		-b $(BOARD_NAME) \
		-p $(PORT)

clean:
	@echo "cleaning..."
	rm -rf $(BUILD_PATH)
	rm -f $(CURDIR)/$(BUILD_FILES_PREFIX).*

install:
	@echo "installing libraries..."
	arduino-cli lib update-index
	arduino-cli lib install $(LIBS)
	@echo "install core..."
	arduino-cli core update-index
	arduino-cli core install $(BOARD_CORE)

print:
	@echo "BOARD_ARCH=$(BOARD_ARCH)"
	@echo "BOARD=$(BOARD)"
	@echo "PORT=$(PORT)"
	@echo "CONFIG_DATE_LAYOUT=$(CONFIG_DATE_LAYOUT)"
	@echo "CONFIG_MEASUREMENT_SYSTEM=$(CONFIG_MEASUREMENT_SYSTEM)"
	@echo "CONFIG_LED_LAYOUT=$(CONFIG_LED_LAYOUT)"
	@echo "CONFIG_USE_SECONDS=$(CONFIG_USE_SECONDS)"
	@echo "CONFIG_USE_DOTS=$(CONFIG_USE_DOTS)"
ifdef CONFIG_USE_SECONDS
	@echo "CONFIG_LED_TIME_LOWER_I2C_ADDR=$(CONFIG_LED_TIME_LOWER_I2C_ADDR)"
	@echo "CONFIG_LED_TIME_UPPER_I2C_ADDR=$(CONFIG_LED_TIME_UPPER_I2C_ADDR)"
else
	@echo "CONFIG_LED_TIME_I2C_ADDR=$(CONFIG_LED_TIME_I2C_ADDR)"
endif
	@echo "CONFIG_LED_MDAY_I2C_ADDR=$(CONFIG_LED_MDAY_I2C_ADDR)"
	@echo "CONFIG_LED_YEAR_I2C_ADDR=$(CONFIG_LED_YEAR_I2C_ADDR)"
	@echo "CONFIG_AM_PIN=$(CONFIG_AM_PIN)"
	@echo "CONFIG_PM_PIN=$(CONFIG_PM_PIN)"
ifeq ($(CONFIG_GPS_DISPLAY), LCD)
	@echo "CONFIG_LCD_DRIVER=$(CONFIG_LCD_DRIVER)"
	@echo "CONFIG_LCD_I2C_ADDR=$(CONFIG_LCD_I2C_ADDR)"
	@echo "CONFIG_LCD_TYPE=$(CONFIG_LCD_TYPE)"
else ifeq ($(CONFIG_GPS_DISPLAY), OLED)
	@echo "CONFIG_OLED_I2C_ADDR=$(CONFIG_OLED_I2C_ADDR)"
	@echo "CONFIG_OLED_SIZE=$(CONFIG_OLED_SIZE)"
endif
	@echo "CONFIG_DIMMER_PIN=$(CONFIG_DIMMER_PIN)"
	@echo "CONFIG_MODE_PIN=$(CONFIG_MODE_PIN)"
	@echo "CONFIG_MODE_DEBOUNCE_MS=$(CONFIG_MODE_DEBOUNCE_MS)"
	@echo "CONFIG_TZ_A_PIN=$(CONFIG_TZ_A_PIN)"
	@echo "CONFIG_TZ_B_PIN=$(CONFIG_TZ_B_PIN)"
	@echo "CONFIG_TZ_BUTTON_PIN=$(CONFIG_TZ_BUTTON_PIN)"
	@echo "CONFIG_TZ_DEBOUNCE_MS=$(CONFIG_TZ_DEBOUNCE_MS)"
	@echo "CONFIG_TZ_ERROR_MS=$(CONFIG_TZ_ERROR_MS)"
	@echo "CONFIG_GPS_RX_PIN=$(CONFIG_GPS_RX_PIN)"
	@echo "CONFIG_GPS_TX_PIN=$(CONFIG_GPS_TX_PIN)"
	@echo "CONFIG_GPS_BAUD_RATE=$(CONFIG_GPS_BAUD_RATE)"
	@echo "CONFIG_AUTO_OFF_MS=$(CONFIG_AUTO_OFF_MS)"

config: $(CONFIG_TARGETS)

.PHONY: FORCE

%.h: .% FORCE
	@echo "generating $@"
	@echo "/*" > $@
	@echo " * This file was automatically generated on $(shell date -R)" >> $@
	@echo " */" >> $@
	@echo "#ifndef __CONFIG_H" >> $@
	@echo "#define __CONFIG_H" >> $@
	@echo "" >> $@
	@echo "// Configuration for date layout." >> $@
	@echo "#define DATE_LAYOUT_$(CONFIG_DATE_LAYOUT)" >> $@
	@echo "" >> $@
	@echo "// Configuration of measurement system." >> $@
	@echo "#define MEASUREMENT_SYSTEM_$(CONFIG_MEASUREMENT_SYSTEM)" >> $@
	@echo "" >> $@
	@echo "// Configuration for LED displays that show date and time." >> $@
	@echo "#define LED_LAYOUT_$(CONFIG_LED_LAYOUT)" >> $@
ifdef CONFIG_USE_SECONDS
	@echo "#define USE_SECONDS" >> $@
endif
ifdef CONFIG_USE_DOTS
	@echo "#define USE_DOTS" >> $@
endif
ifdef CONFIG_USE_SECONDS
	@echo "#define LED_TIME_LOWER_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_TIME_LOWER_I2C_ADDR))" >> $@
	@echo "#define LED_TIME_UPPER_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_TIME_UPPER_I2C_ADDR))" >> $@
else
	@echo "#define LED_TIME_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_TIME_I2C_ADDR))" >> $@
endif
	@echo "#define LED_MDAY_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_MDAY_I2C_ADDR))" >> $@
	@echo "#define LED_YEAR_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_YEAR_I2C_ADDR))" >> $@
	@echo "" >> $@
	@echo "// Configuration for AM/PM pins" >> $@
	@echo "#define AM_PIN static_cast<uint8_t>($(CONFIG_AM_PIN))" >> $@
	@echo "#define PM_PIN static_cast<uint8_t>($(CONFIG_PM_PIN))" >> $@
	@echo "" >> $@
ifeq ($(CONFIG_GPS_DISPLAY), LCD)
	@echo "// Configuration for LCD display that shows GPS information." >> $@
	@echo "#define GPS_DISPLAY_LCD" >> $@
	@echo "#define LCD_DRIVER_$(CONFIG_LCD_DRIVER)" >> $@
	@echo "#define LCD_I2C_ADDR static_cast<uint8_t>($(CONFIG_LCD_I2C_ADDR))" >> $@
	@echo "#define LCD_$(CONFIG_LCD_TYPE)" >> $@
else ifeq ($(CONFIG_GPS_DISPLAY), OLED)
	@echo "// Configuration for OLED display that shows GPS information." >> $@
	@echo "#define GPS_DISPLAY_OLED" >> $@
	@echo "#define OLED_I2C_ADDR static_cast<uint8_t>($(CONFIG_OLED_I2C_ADDR))" >> $@
	@echo "#define OLED_SIZE_$(CONFIG_OLED_SIZE)" >> $@
endif
	@echo "" >> $@
	@echo "// Configuration for light monitor that automatically sets brightness level." >> $@
	@echo "#define DIMMER_PIN static_cast<uint8_t>($(CONFIG_DIMMER_PIN))" >> $@
	@echo "" >> $@
	@echo "// Configuration for 12/24-hour mode selector." >> $@
	@echo "#define MODE_PIN static_cast<uint8_t>($(CONFIG_MODE_PIN))" >> $@
	@echo "#define MODE_DEBOUNCE_MS static_cast<uint32_t>($(CONFIG_MODE_DEBOUNCE_MS))" >> $@
	@echo "" >> $@
	@echo "// Configuration for timezone selector." >> $@
	@echo "#define TZ_A_PIN static_cast<uint8_t>($(CONFIG_TZ_A_PIN))" >> $@
	@echo "#define TZ_B_PIN static_cast<uint8_t>($(CONFIG_TZ_B_PIN))" >> $@
	@echo "#define TZ_BUTTON_PIN static_cast<uint8_t>($(CONFIG_TZ_BUTTON_PIN))" >> $@
	@echo "#define TZ_DEBOUNCE_MS static_cast<uint32_t>($(CONFIG_TZ_DEBOUNCE_MS))" >> $@
	@echo "#define TZ_ERROR_MS static_cast<uint32_t>($(CONFIG_TZ_ERROR_MS))" >> $@
	@echo "" >> $@
	@echo "// Configuration for GPS module." >> $@
	@echo "#define GPS_RX_PIN static_cast<uint8_t>($(CONFIG_GPS_RX_PIN))" >> $@
	@echo "#define GPS_TX_PIN static_cast<uint8_t>($(CONFIG_GPS_TX_PIN))" >> $@
	@echo "#define GPS_BAUD_RATE static_cast<long>($(CONFIG_GPS_BAUD_RATE))" >> $@
	@echo "" >> $@
	@echo "// Configuration for automatically disabling LCD backlight." >> $@
	@echo "#define AUTO_OFF_MS static_cast<uint32_t>($(CONFIG_AUTO_OFF_MS))" >> $@
	@echo "" >> $@
	@echo "#endif" >> $@
