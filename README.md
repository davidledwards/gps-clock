# GPS Clock

A GPS-synchronized digital clock based on the open-source [Arduino](https://arduino.cc) hardware platform.

* [Overview](#overview)
* [Hardware](#hardware)
* [Assembly](#assembly)
* [Software](#software)
* [License](#license)

## Overview

The COVID-19 lockdown rekindled a decades-long interest of mine in learning a bit more about electronics. Even though I was fascinated with the subject at a very early age, my interests gravitated towards software, and 40 years later, here I am clearing the cobwebs and delving into the study of electronics.

The best and most rewarding way to learn something new is to build something tangible and functional. This project is a culmination of my studies and hacking over a period of three months. It is a nontrival project in the sense that a variety of components are needed to build a digital clock that automatically sets the time and date based on GPS signals.

Bear with me if you happen to be a long-time Arduino hacker and developer, as this is my first serious project other than piecing things together with breadboards.

## Hardware

### Generation 1

https://github.com/davidledwards/gps-clock/tree/release-1

These are the components of the original clock design.

* [Elegoo Uno R3](https://www.amazon.com/gp/product/B01EWOE0UU) (1)
* [Adafruit Ultimate GPS Logger Shield](https://www.amazon.com/gp/product/B00E4WEX76) (1)
* [Adafruit 0.56" 4-Digit 7-Segment Display with I2C Backpack](https://www.amazon.com/gp/product/B00XW2L6SS) (3)
* [Adafruit Standard LCD 20x4](https://www.amazon.com/gp/product/B00SK69BZ6) (1)
* [Adafruit I2C LCD Backpack](https://www.amazon.com/gp/product/B00OKCON84) (1)
* [Adafruit Rotary Encoder](https://www.amazon.com/gp/product/B00SK8KK5Y) (1)
* [ElectroCookie Large Solderable PCB](https://www.amazon.com/gp/product/B07YBYZCTN) (1)
* [ElectroCookie Mini Solderable PCB](https://www.amazon.com/gp/product/B081MSKJJX) (1)

### Generation 2

https://github.com/davidledwards/gps-clock/tree/release-2

This generation of the clock included several refinements and improvements.

The time segment of the LED display was changed to green to create a visual distinction with the year/month/day LEDs, which turned out to be a nice usability improvement.

I decided to experiment with a photoresistor that is used to detect the ambient light level and adjust the brightness of the LED displays.

The mini PCB from generation 1 was eliminated since the rotary encoder was mounted directly on the GPS logger shield. There was also ample space on the GPS board to solder the photoresistor and pulldown resistor.

* [Elegoo Uno R3](https://www.amazon.com/gp/product/B01EWOE0UU) (1)
* [Adafruit Ultimate GPS Logger Shield](https://www.amazon.com/gp/product/B00E4WEX76) (1)
* [Adafruit 0.56" 4-Digit 7-Segment Display with I2C Backpack (white)](https://www.adafruit.com/product/1002) (2)
* [Adafruit 0.56" 4-Digit 7-Segment Display with I2C Backpack (green)](https://www.adafruit.com/product/880) (1)
* [JANSANE 20x4 LCD Display](https://www.amazon.com/gp/product/B07D7ZQVDR) (1)
* [Adafruit Rotary Encoder](https://www.amazon.com/gp/product/B00SK8KK5Y) (1)
* [ElectroCookie Large Solderable PCB](https://www.amazon.com/gp/product/B07YBYZCTN) (1)
* [Photoresistor](https://www.amazon.com/Photoresistor/s?k=Photoresistor) (1)
* [10K Ohm Resistor](https://www.amazon.com/10k-ohm-resistor/s?k=10k+ohm+resistor) (1)
* [220 Ohm Resistor](https://www.amazon.com/slp/220-ohm-resistor/pwc2jfx3cwoh9sf) (1)

### Generation 3

https://github.com/davidledwards/gps-clock/tree/release-3

This generation adds a push button to toggle between 12- and 24-hour formats. In 12-hour format, the LED display illuminates a dot on the lower-right edge of the time component. Additionally, it does not zero-pad the hour as seen in the 24-hour format. The goal was to make the format visually obvious to the observer without introducing any additional components.

The format is also stored in EEPROM, which means the clock will remember the last choice should it lose power.

* [Elegoo Uno R3](https://www.amazon.com/gp/product/B01EWOE0UU) (1)
* [Adafruit Ultimate GPS Logger Shield](https://www.amazon.com/gp/product/B00E4WEX76) (1)
* [Adafruit 0.56" 4-Digit 7-Segment Display with I2C Backpack (green)](https://www.adafruit.com/product/880) (2)
* [Adafruit 0.56" 4-Digit 7-Segment Display with I2C Backpack (blue)](https://www.adafruit.com/product/881) (1)
* [JANSANE 20x4 LCD Display](https://www.amazon.com/gp/product/B07D7ZQVDR) (1)
* [Adafruit Rotary Encoder](https://www.amazon.com/gp/product/B00SK8KK5Y) (1)
* [ElectroCookie Large Solderable PCB](https://www.amazon.com/gp/product/B07YBYZCTN) (1)
* [Photoresistor](https://www.amazon.com/Photoresistor/s?k=Photoresistor) (1)
* [10K Ohm Resistor](https://www.amazon.com/10k-ohm-resistor/s?k=10k+ohm+resistor) (1)
* [220 Ohm Resistor](https://www.amazon.com/slp/220-ohm-resistor/pwc2jfx3cwoh9sf) (1)
* [Tactile Button](https://www.adafruit.com/product/367) (1)

### Generation 3 Software Update

I decided to move away from UTC offsets when selecting the timezone using the rotary encoder. Instead, the encoder now moves through a list of predefined timezones with daylight savings rules incorporated. The tradeoff is that given the constrained amount of RAM on the UNO board (2K), only a handful of timezones can be defined. Plans are in place to support the [MEGA](https://www.elegoo.com/products/elegoo-mega-2560-r3-board) board which comes with 8K of RAM, thus allowing a larger set of timezones.

If you decide to clone the repository and modify the selectable timezones, be cautious when adding new entries. Use of too much RAM for global variables reduces the amount of available stack space. Overflowing stack space can result in all kinds of wonky behavior. Once support for the MEGA board is announced, there should be plenty of space to define the most popular timezones around the globe.

A few other cosmetic improvements accompany this software update.

* In 12-hour mode, the dot on the LED to the right of the minute digit would be illuminated to give the user an indication that 12-hour mode was selected. This was necessary in order to distinguish between `10:32 PM` in 12-hour mode versus `10:32 AM` in 24-hour mode. However, it turns out that the dot only needs to be illuminated in 12-hour mode when the hour is PM, i.e. between 12:00 PM and 11:59 PM. This is also consistent with the behavior of most 12-hour clocks.
* When the rotary encoder is moved clockwise or counterclockwise to adjust timezone, the LCD display now provides a visual indicator showing that the timezone has changed but not yet committed (via pressing the button). The label on the LCD display shows `tz?` when selecting and `tz:` when committed. Additionally, the LED display is immediately updated to reflect the local time in the selected timezone. As previously supported, if the encoder detects no movement after 10 seconds, the selection is discarded and reverted back to the last committed timezone.

Internally, the GPS library was replaced with a smaller implementation requiring less memory. Originally, I had used the library built for the Adafruit GPS module, but all the bells and whistles were unnecessary. A generic library supporting the basic standard sentences that all GPS modules emit was sufficient. The upside was additional memory that could be used by the timezone database.

## Assembly

### Circuit Diagram

<img src="images/gps-clock.png" alt="Circuit diagram"/>

### LED Displays

Three 4-digit LED displays are used to show the local time in YYYY.MM.DD HH:MM format. The I2C backpacks are packaged unattached to the LED displays, which means they requiring soldering as part of the assembly process. The I2C backpacks are very convenient because they essentially need only two (2) pins on the Aurduino board for sending commands to the display. Furthermore, since I2C is a serial protocol that acts like a bus, multiple I2C-aware components can be attached to the same pins. However, they do require unique addresses, which will be discussed shortly.

These are the materials that ship with the 4-digit displays.

<img src="images/led-display-unassembled.jpg" alt="Unassembled LED display" style="zoom:25%;" />

Solder the backpack to the display, but make sure it is not attached upside down. The *dots* are a convenient visual aid for aligning to the correct orientation. Solder four (4) of the pin headers to the top of the backpack. Repeat the same process for the remaining displays and backpacks.

This is a view of the assembled LED display.

<img src="images/led-display-assembled.jpg" alt="Assembled LED display" style="zoom:25%;" />

Once the LED displays have been assembled, each needs to be assigned a unique I2C address. This is accomplished by soldering jumpers on the underside of the backpack. There are three (3) jumpers that act like bits, so as one can imagine, there are 8 unique addresses. Each backpack comes with all three jumpers open, which is equivalent to address `0x70`. The `gpsconfig.h` file shows the address chosen for each LED display.

The LED displays are then arranged onto a larger PCB. The YYYY and MMDD displays are essentially touching each other while the HHMM has a small amount of space between its adjacent display. This was more of a design aesthetic than anything else.

This is a view of the LED displays attached to the PCB.

<img src="images/led-pcb.jpg" alt="LED displays attached to PCB" style="zoom:25%;" />

This is the wiring on the front side of the PCB before the LED displays were soldered. Wires from the power rail attach to pins on each display. And, the two pins representing the I2C bus are attached to each display in parallel.

<img src="images/led-pcb-front.jpg" alt="Front side of PCB" style="zoom:25%;" />

Finally, this is the back of the PCB. Note that pin headers for power (+/-) and I2C were attached on both ends of the board. The reason for an additional set of headers is that they are being used to connect the LCD display, which is discussed later.

<img src="images/led-pcb-back.jpg" alt="Back side of PCB" style="zoom:25%;" />

### LCD Display

A single 20-column, 4-row LCD display is used to show GPS information and the currently selected timezone. The I2C backpack for the LCD was sold separately, but as mentioned above, is very convenient in reducing pin consumption on the Arduino board. The pins on the LED display simply connect to the LCD display.

This is a view of the front side of the LCD attached to the backpack.

<img src="images/lcd-front.jpg" alt="Front side of LCD" style="zoom:25%;" />

And, this is a view of the back side of the LCD. Notice that I used the screw headers instead of pin headers. In retrospect, I would have soldered pin headers but did not want the hassle of removing the backpack and resoldering. Since the LCD display is operating on the same I2C pins, it requires an address that does not conflict with the LED displays (`0x70`, `0x71`, `0x72`). If you look closely, the `A0` and `A1` jumpers on the lower left of the backpack are soldered together, making the address of the LCD `0x73`.

<img src="images/lcd-back.jpg" alt="Back side of LCD" style="zoom:25%;" />

#### Generation 2

I decided to purchase a two-pack of the JANSANE 20x4 LCD displays instead of the Adafruit variant used in generation 1. It turns out that this introduced some complications in the source code because the JANSANE and Adafruit displays use different types of I/O expander chips.

The JANSANE uses a PCF8574T and the Adafruit uses a MCP23008. In short, this meant that the library needed to change as well as the I2C address. To further complicate, there are two variants of the JANSANE I/O expander, PCF8574T and PCF8574AT. The former defaults to the I2C address of `0x27` and the latter to `0x3F`. Because the PCF* chips use I2C addresses that do not conflict with the other Adafruit components, there was no need to solder jumpers on the backpack. The source code was modified to support conditional compilation depending on the type of I/O expander, which must be specified in the environment.

The JANSANE backpack attached to the LCD requires a jumper to enable the backlight. Rather than just close the circuit, I decided to use a 220 ohm resistor, which turned out to soften the display a bit.

<img src="images/lcd-jansane-back.jpg" alt="Back side of JANSANE LCD" style="zoom:25%;" />

### Rotary Encoder

The rotary encoder used in this project rotates infinitely in both directions with a nice mechanical pulse as it moves around. It also has a push button action. The encoder is used to select the timezone in 30-minute increments. For a variety of reasons, I kept the hardware and software simple with respect to timezone. Indeed, there are web services that will convert latitude/longitude to timezone, but this clock was designed to be self-contained and not rely on the presence of a wifi endpoint. The timezone offset is selected by rotating the encoder in either direction, but the selection is not committed until the encoder is pressed. Once committed, the local time shown in the LED display is modified accordingly. If a timezone is not selected, local time is always equivalent to UTC. The timezone is also stored in EEPROM so it can be recovered if the power source is interrupted.

As seen in this photo, the rotary encoder is mounted on a small PCB. It could also be mounted to a face plate.

<img src="images/rotary-front.jpg" alt="Front side of rotary encoder" style="zoom:25%;" />

This is a view of the back side of the PCB. Similar to other components, pin headers are used for making connections to the encoder.

<img src="images/rotary-back.jpg" alt="Back side of rotary encoder" style="zoom:25%;" />

#### Generation 2

The rotary encoder was mounted directly on the GPS board, which eliminated the need for a separate mini PCB.

### GPS Module

The GPS module conveniently mounts directly on top of the Arduino Uno board. A set of pin headers need to be soldered to the GPS board, but once completed, both PCBs slide together nicely.

<img src="images/arduino-gps-module.jpg" alt="Arduino Uno and GPS module" style="zoom:25%;" />

This is a view of the GPS module mounted to the Arduino. Notice the additional pin headers I soldered to the GPS board. These are the pins needed to connect the LEDs, LCD and rotary encoder.

<img src="images/gps-module.jpg" alt="GPS module" style="zoom:25%;" />

This is a side view of the GPS mounted to the Arduino. You can see the pin headers for power and those used for the I2C serial bus (`A4`, `A5`).

<img src="images/gps-module-pins-analog.jpg" alt="Side view of GPS mounted to Arduino" style="zoom:25%;" />

This is the other side of the GPS. The visible pin headers (`9`, `10`, `11`) connect to the rotary encoder.

<img src="images/gps-module-pins-digital.jpg" alt="Side view of GPS mounted to Arduino" style="zoom:25%;" />

#### Generation 2

The GPS board radically changed in this generation for two reasons. First, the rotary encoder was mounted directly on the GPS board. Second, the newly introduced photoresistor and its corresponding 10K ohm pulldown resistor were also soldered to the board. This simplified wiring and eliminated the mini PCB. It was also more aesthetically pleasing because the encoder was further recessed and less prominent when compared to generation 1.

The photoresistor is nearly invisible because of its size. It sits adjacent to the GPS receiver near its top-left corner. The pulldown resistor is located just below the photoresistor.

<img src="images/gps-module-front.jpg" alt="Front of GPS module" style="zoom:25%;" />

<img src="images/gps-module-back.jpg" alt="Back of GPS module" style="zoom:25%;" />

#### Generation 3

The top of the GPS board was cleaned up in this generation by moving all wiring to the underside. The rotary encoder has been moved closer to the edge of the PCB to make room for the tactile button, which is used to toggle between 12- and 24-hour format.

<img src="images/gps-3-front.jpg" alt="Front of GPS module" style="zoom:25%;" />

<img src="images/gps-3-back.jpg" alt="Back of GPS module" style="zoom:25%;" />

### Connected Components

This is a view of all components connected together and arranged into a final product. The use of pin headers made it very convenient to delay the majority of decisions about the layout of components in a box or similar structure. However, during the final assembly process, I had to eliminate some of the headers since they were obstructing other components. Also, notice how the rotary encoder is stacked on top of the GPS module.

<img src="images/final-front.jpg" alt="Front view of final assembly" style="zoom:25%;" />

<img src="images/final-top.jpg" alt="Top view of final assembly" style="zoom:25%;" />

<img src="images/final-back.jpg" alt="Back view of final assembly" style="zoom:25%;" />

This is an operating view of the clock with realtime GPS information displayed on the LCD. Once a satellite fix has been established, the LCD shows the latitude and longitude in decimal degrees format. It also displays the number of satellites for which a fix has been established and the UTC time. The selected timezone is also shown, but this is governed by the rotary encoder, not a data point from the GPS module.

<img src="images/final-operating.jpg" alt="Operating view of final assembly" style="zoom:25%;" />

### Generation 2

The second generation turned out to be a slightly nicer design in the final assembly, primarily because the mini PCB was eliminated.

<img src="images/final-2-right.jpg" alt="Right view of final assembly" style="zoom:25%;" />

<img src="images/final-2-left.jpg" alt="Left view of final assembly" style="zoom:25%;" />

<img src="images/final-2-back.jpg" alt="Back view of final assembly" style="zoom:25%;" />

<img src="images/final-2-operating.jpg" alt="Operating view of final assembly" style="zoom:25%;" />

#### Generation 3

Very little changed in comparison to generation 2 except for moving wires to the underside of PCBs. This is also evident on the face of the clock in which prior generations had wires soldered on the front side.

The colors of the LEDs were also changed to green (year/month) and blue (time), but this was mainly due to personal taste.

The operating view below shows 12-hour format enabled.

<img src="images/final-3-front.jpg" alt="Front view of final assembly" style="zoom:25%;" />

<img src="images/final-3-back.jpg" alt="Back view of final assembly" style="zoom:25%;" />

<img src="images/final-3-operating.jpg" alt="Operating view of final assembly" style="zoom:25%;" />


## Software

The [Arduino CLI](https://arduino.cc/pro/cli) is used for both compilation of source code and uploading of the binary to the actual board. I found this CLI preferrable over the [Arduino IDE](https://www.arduino.cc/en/main/software) because it allows the build and upload process to be fully described through the makefile. Simplicity of the development environment was essential. That said, if you prefer an IDE experience, I found the [Arduino for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) extension to be much nicer than Arduino IDE.

Installs the Arduino core and dependent external libraries. This is an idempotent operation that first updates the library index and then ensures that all versioned dependencies are downloaded.

```sh
make install
```

Builds the sketch and its associated C++ files. The output directory of the build cannot be a subdirectory of the project, a restriction imposed by the Arduino compilation process, so it is placed under `TMPDIR`. However, the final program artifacts are written to the project directory.

```sh
make build
```

Several environment variables affect the compilation process. Each of them have default values that may not reflect the hardware components being used.

#### `BOARD_TYPE`

The default board type is `uno`, which is the only officially supported target at this point in time. The `mega` board type is experimental, though compilation and uploads appear to be working just fine.

To change the board type:

```shell
export BOARD_TYPE=mega
```

#### `IO_EXPANDER`

Make sure `IO_EXPANDER` is defined in the environment based on the type of I2C backpack attached to the LCD display, as this affects the build. The makefile defaults to `PCF8574T`.

For Adafruit LCD display:

```sh
export IO_EXPANDER=USE_MCP23008
```

For JANSANE LCD display (two types, so verify which chip is attached to the backpack):

```sh
export IO_EXPANDER=USE_PCF8574T
```

or

```sh
export IO_EXPANDER=USE_PCF8574AT
```

#### `PORT`

Uploads the program to the Arduino board. Make sure `PORT` is defined by the environment or provided as an argument to `make`. `PORT` is the serial port to which the Arduino board is attached. If undefined, it defaults to `/dev/null` and will cause an upload attempt to fail.

```sh
make upload
```

```sh
make upload PORT=/dev/tty.usbmodem14401
```

```sh
export PORT=/dev/tty.usbmodem14401
make upload
```

Removes transient build files.

```sh
make clean
```

Performs an installation of libraries followed by a build and upload of the final program to the Arduino board.

```sh
make all
```

## License

Copyright 2020 David Edwards

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

<http://www.apache.org/licenses/LICENSE-2.0>

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
