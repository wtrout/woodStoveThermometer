#include <Arduino.h>
#include <U8x8lib.h>

constexpr byte therms[] = {A3, A2};
constexpr byte button   = 1;

U8X8_SSD1306_128X32_UNIVISION_SW_I2C oled(SCL, SDA);


//              ┌─────u─────┐
//          NC -│RST     VCC│- 3.3V
//              │           │
//     Therm 1 -│PB3     SCL│- OLED
//              │           │
//     Therm 2 -│PB4     PB1│- User Input
//              │           │
//         GND -│GND     SDA│- OLED
//              └───────────┘


void displayTemps(int (&temps)[2]) {

    oled.setCursor(0,1);
    if(temps[0] <  10) {oled.print(" ");}
    if(temps[0] < 100) {oled.print(" ");}
    oled.print(temps[0]);

    oled.setCursor(7,1);
    oled.print(":");

    oled.setCursor(8,1);
    if(temps[1] <  10) {oled.print(" ");}
    if(temps[1] < 100) {oled.print(" ");}
    oled.print(temps[1]);

}

void readTemps(int (&temps)[2], byte count) {

    for(byte i=0; i<2; i++) {
        temps[i] = 0;
        analogRead(therms[i]);
        delay(5);

        for(byte n=0; n<count; n++) {
            temps[i] += analogRead(therms[i]);
        }
        temps[i] /= count;
        temps[i] *= 20;
        temps[i] /= 31;
        temps[i] -= 250;
        temps[i] = constrain(temps[i], 0, 999);
    }
}

void setup() {
    oled.begin();
    oled.setFont(u8x8_font_px437wyse700a_2x2_n);
    oled.setFlipMode(true);
}


void loop() {
    constexpr int readPeriod = 5000;
    static long lastRead = millis() - readPeriod;
    static int temps[2];

    if(millis() - lastRead >= readPeriod) {
        readTemps(temps, 20);
        displayTemps(temps);
        lastRead = millis();
    }
}
