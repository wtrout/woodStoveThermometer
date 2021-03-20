#include <Arduino.h>

// display
// 2x thermocouples
// blower status


//   LED    Seg     Col    Ard
//  ┌─────┬──────┬───────┬─────┐             D2   A   F  D1  D0   B         ┌───────────┐
//  │  1 ┄┼┄ E  ┄┼┄ grn ┄┼┄  4 │             12  11  10   9   8   7         │     A     │
//  │  2 ┄┼┄ D  ┄┼┄ org ┄┼┄  3 │          ┌───┴───┴───┴───┴───┴───┴───┐     │   ┌───┐   │
//  │  3 ┄┼┄ DP ┄┼┄ red ┄┼┄  7 │          │                           │     │ F │   │ B │
//  │  4 ┄┼┄ C  ┄┼┄ blk ┄┼┄  2 │          │     D2      D1     D0     │     │   └───┘   │
//  │  5 ┄┼┄ G  ┄┼┄ brn ┄┼┄  6 │          │                           │     │     G     │
//  │     │      │       │     │          └─────┬───┬───┬───┬───┬─────┘     │   ┌───┐   │
//  │  7 ┄┼┄ B  ┄┼┄ pur ┄┼┄  1 │                1   2   3   4   5           │ E │   │ C │
//  │  8 ┄┼┄ D0 ┄┼┄ GRN ┄┼┄  8 │                E   D  DP   C   G           │   └───┘   │ ┌──┐
//  │  9 ┄┼┄ D1 ┄┼┄ RED ┄┼┄  9 │                                            │     D     │ │DP│
//  │ 10 ┄┼┄ F  ┄┼┄ yel ┄┼┄  5 │                                            └───────────┘ └──┘
//  │ 11 ┄┼┄ A  ┄┼┄ blu ┄┼┄  0 │
//  │ 12 ┄┼┄ D2 ┄┼┄ BLU ┄┼┄ 10 │ 
//  └─────┴──────┴───────┴─────┘
//
//

constexpr byte bMask = ~0b00000111;
constexpr byte anodeCount    = 8;
constexpr byte cathodeCount  = 3;
constexpr byte digits[] = { // 0b .GFE DCBA
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x67, // 9
};



void writeDigit(byte digit, byte position) {
    constexpr byte holdTime = 5;

    PORTB = ( (PORTB & bMask) | (0b00000001 << position) );
    PORTD = digits[digit];
    delay(holdTime);
}

bool writeNumber(int number) {
    if (number > 999) { return false; }

    if (number > 99) {
        writeDigit(number / 100, 2);
    }
    writeDigit((number % 100) / 10, 1);
    writeDigit(number % 10, 0);

    return true;
}

void readTemps(int (&temps)[2], byte count) {
    long temp0 = 0;
    long temp1 = 0;

    analogRead(A2);
    delay(5);
    for(byte i=0; i<count; i++) {
        temp0 += analogRead(A2);
    }
    temp0 *= 5000;
    temp0 /= count;
    temp0 /= 1023;
    temp0 -= 1250;
    temps[0] = temp0 / 5;

    analogRead(A3);
    delay(5);
    for(byte i=0; i<count; i++) {
        temp1 += analogRead(A3);
    }
    temp1 *= 5000;
    temp1 /= count;
    temp1 /= 1023;
    temp1 -= 1250;
    temps[1] = temp1 / 5;
}

void displayTemps(byte (&temps)[2]) {
    byte n = 0;
    writeNumber(temps[n]);
}


void setup() {
    DDRD  = 0xFF; // pins 1-7 for segment anodes
    DDRB  = 0x0F; // pins 8-10 for segment cathodes + 11 for pwm dimming

    //TCCR2A = 0x83; // 1000 0011
    //TCCR2B = 0x02; // 0000 0010
    //TIMSK2 = 0x00;
    //OCR2A  = 0xFF; // 1111 1111
}

void loop() {
    static int temps[2] = {0,0};
    constexpr int readPeriod = 3000;
    static long lastRead = millis() - readPeriod;
    static bool flip = 0;



    writeNumber(temps[flip]);

    if(millis() - lastRead >= readPeriod) {
        readTemps(temps, 10);
        flip = !flip;
        lastRead = millis();
    }
}