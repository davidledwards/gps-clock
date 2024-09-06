# Changelog

This file contains relevant information for each release. Please refer to the commit history for more details.

Some of the tags are not included in this changelog due to their redundant nature. This redundacy was due to an historical decision to manage multiple generational release streams. Beginning with version `4.0`, only a single releaase stream is being managed. The software is backward-compatible with all prior generations assuming proper configuration.

## [5.2](https://github.com/davidledwards/gps-clock/tree/5.2) - `???`

### Added

- Add support for [Arduino Nano 33 BLE](https://docs.arduino.cc/hardware/nano-33-ble/)
- Add support for [Arduino Nano Every](https://docs.arduino.cc/hardware/nano-every/)
- Add predefined configurations for new boards

### Changed

- Change default I2C address for OLEDs from `0x78` to `0x3C`
- Remove `CONFIG_OLED_DRIVER` configuration since no longer needed
- Remove `CONFIG_I2C_CLOCK_PIN` and `CONFIG_I2C_DATA_PIN` configuration since no longer needed
- Refactor LCD and OLED code to use new libraries

## [5.1](https://github.com/davidledwards/gps-clock/tree/5.1) - `2024-08-28`

### Added

- Add support for 128x32 OLED display

## [5.0](https://github.com/davidledwards/gps-clock/tree/5.0) - `2024-08-26`

### Added

- Add support for 128x64 OLED display
- Add support for [Arduino Nano 33 IoT](https://docs.arduino.cc/hardware/nano-33-iot/)
- Add support for EEPROM emulation on boards with only flash memory
- More intuitive and exhaustive list of predefined configurations
- Add configuration option for _imperial_ or _metric_ measurement system
- Add _altitude_ to GPS display

### Changed

- Change layout of LCD display to use glyphs instead of labels
- Modify names of several configuration options aimed at improving consistency

### Fixed

- Fix error in schematics where `A4` and `A5` pins were swapped

## [4.5](https://github.com/davidledwards/gps-clock/tree/4.5) - `2022-12-24`

### Added

- Upgrade TinyGPS library to TinyGPS++ for compatiblity with newer GPS modules that emit `GN*` sentences

## [4.4](https://github.com/davidledwards/gps-clock/tree/4.4) - `2021-11-14`

### Added

- Use `make` to generate `config.h` based on configuration options
- Add _seconds_ to GPS time on LCD display
- Add support for ISO/EU/US date formats
- Add support for _seconds_ on LED display
- Add support for various LED layouts
- Include predefined configurations
- Update documentation with all configuration options

### Changed

- Remove "UTC" label from LCD display since this is implied
- Remove "TZ" label from LCD display since it feels redundant

### Fixed

- Fix bug that was clobbering LCD display

## [4.3](https://github.com/davidledwards/gps-clock/tree/4.3) - `2021-10-31`

### Added

- Add EET time zone
- Include section in README of clock diagrams from other developers

## [4.2](https://github.com/davidledwards/gps-clock/tree/4.2) - `2021-08-27`

### Fixed

- Only show dot in 12-hour mode when hour is essentially PM

## [4.1](https://github.com/davidledwards/gps-clock/tree/4.1) - `2021-08-27`

### Changed

- Used conditional compilation in lieu of central configuration file
- Detect various I/O expanders to ensure proper LCD library is used

## [4.0](https://github.com/davidledwards/gps-clock/tree/4.0) - `2021-07-24`

### Added

- Initial support for displaying _seconds_
- Add support for Arduino Mega board
- Expand available time zones when compiling for Mega board

### Fixed

- Use pull-up resistor for 12/24-hour button

## [3.2](https://github.com/davidledwards/gps-clock/tree/3.2) - `2021-01-24`

### Added

- Automatically turn on LCD backlight when time zone selector is rotated and turn off after 30 seconds of no activity

### Fixed

- Bug in time zone selector that continued returning proposed time zone after expiration of idle period

## [3.1](https://github.com/davidledwards/gps-clock/tree/3.1) - `2020-12-23`

### Changed

- Updated libraries
- Accommodate changes to Arduino CLI

## [3.0](https://github.com/davidledwards/gps-clock/tree/3.0) - `2020-09-21`

### Added

- Add support for selecting 12/24-hour format

### Changed

- Change default time format on LED display to 12-hour
- Adjust debounce delay for rotary encoder used for selecting time zone

## [2.1](https://github.com/davidledwards/gps-clock/tree/2.1) - `2020-09-07`

### Changed

- Update libraries
- Adjust debounce delay for rotary encoder

## [2.0](https://github.com/davidledwards/gps-clock/tree/2.0) - `2020-08-15`

### Added

- Use photoresistor to automatically adjust brightness of LED display

## [1.0](https://github.com/davidledwards/gps-clock/tree/1.0) - `2020-07-18`

### Added

- First generation of hardware and software
