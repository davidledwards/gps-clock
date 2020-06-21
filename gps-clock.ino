/*
 * Copyright 2020 David Edwards
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_LiquidCrystal.h"
#include "Adafruit_GPS.h"
#include "SoftwareSerial.h"
#include "SimpleRotary.h"
#include "gps-config.h"

#define KNOW_ROTATE_IDLE 0
#define KNOB_ROTATE_FORWARD 1
#define KNOB_ROTATE_BACKWARD 2

#define KNOB_BUTTON_IDLE 0
#define KNOB_BUTTON_PRESSED 1

#define DISP1_ADDR 0x70
#define DISP2_ADDR 0x71
#define DISP3_ADDR 0x72
#define LCD_ADDR_OFFSET 3

Adafruit_7segment disp1 = Adafruit_7segment();
Adafruit_7segment disp2 = Adafruit_7segment();
Adafruit_7segment disp3 = Adafruit_7segment();

Adafruit_LiquidCrystal lcd(LCD_ADDR_OFFSET);

SoftwareSerial ser(8, 7);
Adafruit_GPS gps(&ser);

SimpleRotary knob(9, 10, 11);

long counter = 0;
bool colon = false;

int pos = 5000;

void setup() {
  Serial.begin(115200);
  Serial.print("sizeof(int) = ");
  Serial.println(sizeof(int));
  disp1.begin(DISP1_ADDR);
  disp1.setBrightness(0);
  disp2.begin(DISP2_ADDR);
  disp2.setBrightness(0);
  disp3.begin(DISP3_ADDR);
  disp3.setBrightness(0);
  display(disp3, pos);

  lcd.begin(20, 4);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Welcome to GPS Clock");

  gps.begin(9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  gps.sendCommand(PGCMD_ANTENNA);

  knob.setErrorDelay(200);
}

void loop() {
  long low = counter % 10000;
  long high = counter / 10000;
  display(disp1, high);
  display(disp2, low);
  counter = (counter + 1) % 100000000;

  int rotated = knob.rotate();
  int pushed = knob.push();
  if (pushed == KNOB_BUTTON_PRESSED) {
    pos = 5000;
    display(disp3, pos);
  } else if (rotated == KNOB_ROTATE_FORWARD) {
    pos = (pos + 1) % 10000;
    display(disp3, pos);
  } else if (rotated == KNOB_ROTATE_BACKWARD) {
    if (--pos < 0)
      pos = 9999;
    display(disp3, pos);
  }

  delay(2);
}

static void display(Adafruit_7segment &disp, long value) {
  disp.writeDigitNum(0, value / 1000 % 10);
  disp.writeDigitNum(1, value / 100 % 10);
  disp.writeDigitNum(3, value / 10 % 10);
  disp.writeDigitNum(4, value % 10);
  disp.writeDisplay();
}
