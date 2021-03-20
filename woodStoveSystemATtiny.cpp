#include <Arduino.h>
#include <TinyWireM.h>

#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>

// SDA
// SCL
// Analog 1
// Analog 2
// 3 position

//TODO: Tiny4kOLED


//              ┌───────────┐
//          NC -│RST     VCC│- 5V
//              │           │
//     Therm 1 -│PB3     SCL│- OLED
//              │           │
//     Therm 2 -│PB4     PB1│- User Input
//              │           │
//         GND -│GND     SDA│- OLED
//              └───────────┘


void displayTemps(byte (&temps)[2]) {
    static bool flip = 0;

    oled.setCursor(0,2);
    oled.print(flip);
    oled.print(" ");
    oled.print(temps[flip]);
    oled.display();

    flip = !flip;
}

void readTemps(byte (&temps)[2], byte count) {
    long temp0 = 0;
    long temp1 = 0;

    analogRead(A3);
    delay(5);
    for(byte i=0; i<count; i++) {
        temp0 += analogRead(A3);
    }
    temp0 *= 5000;
    temp0 /= count;
    temp0 /= 1023;
    temp0 -= 1250;
    temps[0] = temp0 / 5;

    analogRead(A2);
    delay(5);
    for(byte i=0; i<count; i++) {
        temp1 += analogRead(A2);
    }
    temp1 *= 5000;
    temp1 /= count;
    temp1 /= 1023;
    temp1 -= 1250;
    temps[1] = temp1 / 5;
}

void setup() {

    oled.begin();
    oled.setFont(FONT8X16);
    oled.clear();
    oled.on();
    

    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oled.setTextSize(4);

}

void loop() {
    constexpr int readPeriod = 5000;
    static long lastRead = millis() - readPeriod;
    static byte temps[2];

    if(millis() - lastRead >= readPeriod) {
        readTemps(temps, 20);
        displayTemps(temps);
        lastRead = millis();
    }
}
