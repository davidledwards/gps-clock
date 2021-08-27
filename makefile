#
# Copyright 2020 David Edwards
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

# This variable specifies the I/O expander used by the LCD display I2C interface, which is used to
# determine the I2C address as well as the library.
#
# Recognized options:
# PCF8574T (default)
# PCF8574AT
# MCP23008
EXPANDER ?= PCF8574T

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

# Construct build flags sent to compiler based on environment.
BUILD_FLAGS = -DEXPANDER=EXPANDER_$(EXPANDER)

ifdef USE_SECONDS
BUILD_FLAGS += -DUSE_SECONDS
endif

help :
	@echo "useful targets:"
	@echo "  all       performs clean, install, build, upload"
	@echo "  install   install library and core dependencies"
	@echo "  build     build sketch"
	@echo "  upload    upload program to board"
	@echo "  clean     remove all transient build files"
	@echo ""
	@echo "environment:"
	@echo "  BOARD=$(BOARD)"
	@echo "  EXPANDER=$(EXPANDER)"
	@echo "  PORT=$(PORT)"
	@echo "  USE_SECONDS=$(USE_SECONDS)"

all : clean install build upload

$(PROG) : $(SRCS)
	@echo "building..."
	arduino-cli compile \
		--build-path $(BUILD_PATH) \
		--build-cache-path $(BUILD_PATH) \
		--build-property build.extra_flags="$(BUILD_FLAGS)" \
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

install-libs :
	@echo "installing libraries..."
	arduino-cli lib update-index
	arduino-cli lib install $(LIBS)

install-core :
	@echo "install core..."
	arduino-cli core update-index
	arduino-cli core install $(CORE)

install : install-core install-libs
