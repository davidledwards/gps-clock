SKETCH = gps-clock
BOARD_CORE = arduino:avr
BOARD_NAME = $(BOARD_CORE):uno
PORT = /dev/tty.usbmodem14401

BUILD_ROOT = $(abspath $(TMPDIR)/build)
BUILD_PATH = $(abspath $(BUILD_ROOT)/$(subst /,_,$(CURDIR)))

BUILD_FILES_PREFIX = $(SKETCH).$(subst :,.,$(BOARD_NAME))

LIBS = \
	"Adafruit LED Backpack Library"@1.1.7 \
	"Adafruit LiquidCrystal"@1.0.1 \
	"Adafruit GPS Library"@1.5.1

CORE = $(BOARD_CORE)@1.8.3

SRCS = $(wildcard *.ino *.h *.cpp)

PROG = $(BUILD_FILES_PREFIX).hex

compile : $(PROG)
	@echo "compiling..."

$(PROG) : $(SRCS)
	arduino-cli compile \
		--build-path $(BUILD_PATH) \
		--build-cache-path $(BUILD_PATH) \
		-b $(BOARD_NAME)

upload :
	arduino-cli upload \
		-b $(BOARD_NAME) \
		-p $(PORT)

clean :
	rm -rf $(BUILD_PATH)
	rm -f $(CURDIR)/$(BUILD_FILES_PREFIX).*

install-libs :
	arduino-cli lib install $(LIBS)

install-core :
	arduino-cli core update-index
	arduino-cli core install $(CORE)

install : install-core install-libs
