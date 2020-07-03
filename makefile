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
SKETCH = gps-clock
BOARD_CORE = arduino:avr
BOARD_NAME = $(BOARD_CORE):uno

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
	"Adafruit LiquidCrystal"@1.1.0 \
	"Adafruit GPS Library"@1.5.1 \
	"SimpleRotary"@1.1.2 \
	"Time"@1.6.0

# Arduino core library dependency.
CORE = $(BOARD_CORE)@1.8.3

# Name of the image that gets uploaded to the board.
PROG = $(BUILD_FILES_PREFIX).hex

help :
	@echo "useful targets:"
	@echo "  all       performs clean, install, build, upload"
	@echo "  install   install library and core dependencies"
	@echo "  build     build sketch"
	@echo "  upload    upload program to board"
	@echo "  clean     remove all transient build files"

all : clean install build upload

SRCS = $(wildcard *.ino *.h *.cpp)

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
