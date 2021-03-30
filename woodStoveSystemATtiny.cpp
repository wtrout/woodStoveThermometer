#include <Arduino.h>
#include <Tiny4kOLED.h>
#include "font16x32digits.h"

constexpr byte therms[] = {A3, A2};
constexpr byte button   = 1;



//              ┌─────u─────┐
//          NC -│RST     VCC│- 3.3V
//              │           │
//     Therm 1 -│PB3     SCL│- OLED + 4.7k pullup
//              │           │
//     Therm 2 -│PB4     PB1│- NC
//              │           │
//         GND -│GND     SDA│- OLED + 4.7k pullup
//              └───────────┘
//
// Therm x is AD8495 thermocouple amplifier; T (°C) = (V - 1.25) * 20
// OLED is ssd1306 with a monochrome 128x32 OLED, 4 pin type (with 3.3v, gnd, SDA and SCL; designed for Raspberry Pi)

void displayTemps(int (&temps)[2]) {
    byte xPos = 0;

    oled.clear();

    if(temps[0] <  10) {xPos+=16;}
    if(temps[0] < 100) {xPos+=16;}
    oled.setCursor(xPos, 0);
    oled.print(temps[0]);

    xPos = 80;

    if(temps[1] <  10) {xPos+=16;}
    if(temps[1] < 100) {xPos+=16;}
    oled.setCursor(xPos, 0);
    oled.print(temps[1]);

    oled.switchFrame();
}

void readTemps(int (&temps)[2], byte count) {

    for(byte i=0; i<2; i++) {
        float read = 0;
        analogRead(therms[i]);
        delay(5);

        for(byte n=0; n<count; n++) {
            read += analogRead(therms[i]);
        }
        read /= count;
        read *= 3300;
        read /= 1023;
        read -= 1250;
        read /= 5;

        read = constrain(read, 0, 999);
        temps[i] = read;
    }
}

void setup() {
    pinMode(5, INPUT_PULLUP);
    pinMode(1, INPUT_PULLUP);

    oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
    oled.setRotation(0);
    oled.setFont(FONT16X32DIGITS);
    oled.on();
    oled.switchRenderFrame();
}

void loop() {
    constexpr int readPeriod = 5000;
    static long lastRead = millis() - readPeriod;
    static int temps[2];

    if(millis() - lastRead >= readPeriod) {
        readTemps(temps, 60);
        displayTemps(temps);
        lastRead = millis();
    }
}
