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
ifneq (,$(wildcard ./.env))
	include .env
	export
endif

SKETCH = gps-clock
BOARD_PACKAGE = arduino
BOARD_ARCH = avr
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
	"Adafruit LED Backpack Library"@1.1.7 \
	"LiquidCrystal I2C"@1.1.2 \
	"Adafruit LiquidCrystal"@1.1.0 \
	"TinyGPS"@13.0.0 \
	"SimpleRotary"@1.1.2 \
	"Time"@1.6.1 \
	"Timezone"@1.2.4 \
	"ezButton"@1.0.3

# Arduino core library dependency.
CORE = $(BOARD_CORE)@1.8.3

# Name of the image that gets uploaded to the board.
PROG = $(BUILD_FILES_PREFIX).hex

SRCS = $(wildcard *.ino *.h *.cpp)

# This section defines configuration variables which are used to generate configuration
# header files. In most cases, variables are assigned default values if undefined.
ifneq (,$(wildcard ./.config))
	include .config
	export
endif

# Configuration for date layout.
CONFIG_DATE_LAYOUT ?= ISO

# Configuration for LED displays that show date and time.
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

CONFIG_LED_LAYOUT ?= NORMAL

# Configuration for LCD display that shows GPS information.
CONFIG_LCD_EXPANDER ?= PCF8574T

ifeq ($(CONFIG_LCD_EXPANDER), PCF8574T)
CONFIG_LCD_I2C_ADDR ?= 0x27
CONFIG_LCD_TYPE ?= GENERIC
else ifeq ($(CONFIG_LCD_EXPANDER), PCF8574AT)
CONFIG_LCD_I2C_ADDR ?= 0x3F
CONFIG_LCD_TYPE ?= GENERIC
else ifeq ($(CONFIG_LCD_EXPANDER), MCP23008)
CONFIG_LCD_I2C_ADDR ?= 0x73
CONFIG_LCD_TYPE ?= ADAFRUIT
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
ifeq ($(BOARD), mega)
CONFIG_GPS_RX_PIN_DEFAULT = 50
CONFIG_GPS_TX_PIN_DEFAULT = 51
else
CONFIG_GPS_RX_PIN_DEFAULT = 7
CONFIG_GPS_TX_PIN_DEFAULT = 8
endif

CONFIG_GPS_RX_PIN ?= $(CONFIG_GPS_RX_PIN_DEFAULT)
CONFIG_GPS_TX_PIN ?= $(CONFIG_GPS_TX_PIN_DEFAULT)
CONFIG_GPS_BAUD_RATE ?= 9600

# Configuration for automatically disabling LCD backlight.
CONFIG_AUTO_OFF_MS ?= 30000

help :
	@echo "useful targets:"
	@echo "  install        install library and core dependencies"
	@echo "  build          build sketch"
	@echo "  upload         upload program to board"
	@echo "  clean          remove all transient build files"
	@echo "  create-config  generate configuration file"
	@echo "  list-config    print configuration variables"
	@echo "  list-env       print environment variables"

$(PROG) : $(SRCS)
	@echo "building..."
	arduino-cli compile \
		--build-path $(BUILD_PATH) \
		--build-cache-path $(BUILD_PATH) \
		-b $(BOARD_NAME)

build : $(PROG)

upload : build
	@echo "uploading to ${PORT}..."
	arduino-cli upload \
	  --input-dir $(BUILD_PATH) \
		-b $(BOARD_NAME) \
		-p $(PORT)

clean :
	@echo "cleaning..."
	rm -rf $(BUILD_PATH)
	rm -f $(CURDIR)/$(BUILD_FILES_PREFIX).*

install :
	@echo "installing libraries..."
	arduino-cli lib update-index
	arduino-cli lib install $(LIBS)
	@echo "install core..."
	arduino-cli core update-index
	arduino-cli core install $(CORE)

list-config :
	@echo "CONFIG_DATE_LAYOUT=$(CONFIG_DATE_LAYOUT)"
	@echo "CONFIG_USE_SECONDS=$(CONFIG_USE_SECONDS)"
ifdef CONFIG_USE_SECONDS
	@echo "CONFIG_LED_TIME_LOWER_I2C_ADDR=$(CONFIG_LED_TIME_LOWER_I2C_ADDR)"
	@echo "CONFIG_LED_TIME_UPPER_I2C_ADDR=$(CONFIG_LED_TIME_UPPER_I2C_ADDR)"
else
	@echo "CONFIG_LED_TIME_I2C_ADDR=$(CONFIG_LED_TIME_I2C_ADDR)"
endif
	@echo "CONFIG_LED_MDAY_I2C_ADDR=$(CONFIG_LED_MDAY_I2C_ADDR)"
	@echo "CONFIG_LED_YEAR_I2C_ADDR=$(CONFIG_LED_YEAR_I2C_ADDR)"
	@echo "CONFIG_LED_LAYOUT=$(CONFIG_LED_LAYOUT)"
	@echo "CONFIG_LCD_EXPANDER=$(CONFIG_LCD_EXPANDER)"
	@echo "CONFIG_LCD_I2C_ADDR=$(CONFIG_LCD_I2C_ADDR)"
	@echo "CONFIG_LCD_TYPE=$(CONFIG_LCD_TYPE)"
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

list-env :
	@echo "BOARD=$(BOARD)"
	@echo "PORT=$(PORT)"

create-config :
	@echo "generating config.h"
	@echo "/*" > config.h
	@echo " * This file was automatically generated on $(shell date -R)" >> config.h
	@echo " */" >> config.h
	@echo "#ifndef __CONFIG_H" >> config.h
	@echo "#define __CONFIG_H" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for date layout." >> config.h
	@echo "#define DATE_LAYOUT_$(CONFIG_DATE_LAYOUT)" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for LED displays that show date and time." >> config.h
ifdef CONFIG_USE_SECONDS
	@echo "#define USE_SECONDS" >> config.h
	@echo "#define LED_TIME_LOWER_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_TIME_LOWER_I2C_ADDR))" >> config.h
	@echo "#define LED_TIME_UPPER_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_TIME_UPPER_I2C_ADDR))" >> config.h
else
	@echo "#undef USE_SECONDS" >> config.h
	@echo "#define LED_TIME_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_TIME_I2C_ADDR))" >> config.h
endif
	@echo "#define LED_MDAY_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_MDAY_I2C_ADDR))" >> config.h
	@echo "#define LED_YEAR_I2C_ADDR static_cast<uint8_t>($(CONFIG_LED_YEAR_I2C_ADDR))" >> config.h
	@echo "#define LED_LAYOUT_$(CONFIG_LED_LAYOUT)" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for LCD display that shows GPS information." >> config.h
	@echo "#define LCD_EXPANDER_$(CONFIG_LCD_EXPANDER)" >> config.h
	@echo "#define LCD_I2C_ADDR static_cast<uint8_t>($(CONFIG_LCD_I2C_ADDR))" >> config.h
	@echo "#define LCD_$(CONFIG_LCD_TYPE)" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for light monitor that automatically sets brightness level." >> config.h
	@echo "#define DIMMER_PIN static_cast<uint8_t>($(CONFIG_DIMMER_PIN))" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for 12/24-hour mode selector." >> config.h
	@echo "#define MODE_PIN static_cast<uint8_t>($(CONFIG_MODE_PIN))" >> config.h
	@echo "#define MODE_DEBOUNCE_MS static_cast<uint32_t>($(CONFIG_MODE_DEBOUNCE_MS))" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for timezone selector." >> config.h
	@echo "#define TZ_A_PIN static_cast<uint8_t>($(CONFIG_TZ_A_PIN))" >> config.h
	@echo "#define TZ_B_PIN static_cast<uint8_t>($(CONFIG_TZ_B_PIN))" >> config.h
	@echo "#define TZ_BUTTON_PIN static_cast<uint8_t>($(CONFIG_TZ_BUTTON_PIN))" >> config.h
	@echo "#define TZ_DEBOUNCE_MS static_cast<uint32_t>($(CONFIG_TZ_DEBOUNCE_MS))" >> config.h
	@echo "#define TZ_ERROR_MS static_cast<uint32_t>($(CONFIG_TZ_ERROR_MS))" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for GPS module." >> config.h
	@echo "#define GPS_RX_PIN static_cast<uint8_t>($(CONFIG_GPS_RX_PIN))" >> config.h
	@echo "#define GPS_TX_PIN static_cast<uint8_t>($(CONFIG_GPS_TX_PIN))" >> config.h
	@echo "#define GPS_BAUD_RATE static_cast<long>($(CONFIG_GPS_BAUD_RATE))" >> config.h
	@echo "" >> config.h
	@echo "// Configuration for automatically disabling LCD backlight." >> config.h
	@echo "#define AUTO_OFF_MS static_cast<uint32_t>($(CONFIG_AUTO_OFF_MS))" >> config.h
	@echo "" >> config.h
	@echo "#endif" >> config.h
