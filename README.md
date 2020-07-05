# GPS Clock

A GPS-synchronized digital clock based on the open-source [Arduino](https://arduino.cc) hardware platform.

## Overview

The COVID-19 lockdown rekindled a decades-long interest of mine in learning a bit more about electronics. Even though I was fascinated with the subject at a very early age, my interests gravitated towards software, and 40 years later, here I am clearing the cobwebs and delving into the study of electronics.

The best and most rewarding way to learn something new is to build something tangible and functional. This project is a culmination of my studies and hacking over a period of three months. It is a nontrival project in the sense that a variety of components are needed to build a digital clock that automatically sets the time and date based on GPS signals.

Bear with me if you happen to be a long-time Arduino hacker and developer, as this is my first serious project other than piecing things together with breadboards.

## Hardware

* [Elegoo Uno R3](https://www.amazon.com/gp/product/B01EWOE0UU) (1)
* [Adafruit Ultimate GPS Logger Shield](https://www.amazon.com/gp/product/B00E4WEX76) (1)
* [Adafruit 0.56" 4-Digit 7-Segment Display with I2C Backpack](https://www.amazon.com/gp/product/B00XW2L6SS) (3)
* [Adafruit Standard LCD 20x4](https://www.amazon.com/gp/product/B00SK69BZ6) (1)
* [Adafruit I2C LCD Backpack](https://www.amazon.com/gp/product/B00OKCON84) (1)
* [Adafruit Rotary Encoder](https://www.amazon.com/gp/product/B00SK8KK5Y) (1)
* [ElectroCookie Solderable PCB](https://www.amazon.com/gp/product/B07YBYZCTN) (1)

## Assembly

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
