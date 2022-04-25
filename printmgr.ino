#include <PS2Keyboard.h>

#define DEBUG 1
#if DEBUG == 1
    #define DEBUG_BEGIN(x) Serial2.begin(x)
    #define DEBUG_PRINT(x) Serial2.print(x)
    #define DEBUG_PRINTLN(x) Serial2.println(x)
#else
    #define DEBUG_BEGIN(x)
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif



// parallel port pin# = arduino pin#
const int pStrobe = 4;
const int pData0 = 5;
const int pData1 = 6;
const int pData2 = 7;
const int pData3 = 8;
const int pData4 = 9;
const int pData5 = 10;
const int pData6 = 11;
const int pData7 = 12;
const int pAck = 13;
const int pBusy = 14; //A0 on Arduino Nano since there is no D13 pin
// keyboard pins
const int kData = 3;
const int kClk = 2;

//Microseconds to strobe nStrobe for
const int strobeWait = 2;

const int lineLen = 80;
int linePos = 1;
const byte initMessage[] = "Initializing...\r\n";

bool isEscape = 0;
bool isPeeked = 0;
unsigned int time = 0;
const unsigned int peekMaxDelay = 6;

PS2Keyboard keyboard;

void setup() {
    Serial.begin(9600); // Limit baudrate to help slow down data transfers so data is less likely to be missed
    DEBUG_BEGIN(115200); // Debug serial connection
    keyboard.begin(kData,kClk);

    pinMode(pStrobe, OUTPUT);      // is active LOW
    digitalWrite(pStrobe, HIGH);   // set HIGH
    pinMode(pData0, OUTPUT);
    pinMode(pData1, OUTPUT);
    pinMode(pData2, OUTPUT);
    pinMode(pData3, OUTPUT);
    pinMode(pData4, OUTPUT);
    pinMode(pData5, OUTPUT);
    pinMode(pData6, OUTPUT);
    pinMode(pData7, OUTPUT);

    pinMode(pAck, INPUT);     // is active LOW
    pinMode(pBusy, INPUT);

    delay(1000);

    timerSetup();

    resetPrinter();
    printMessage(initMessage);

    pinMode(LED_BUILTIN,OUTPUT);

    DEBUG_PRINTLN("Startup complete");
}

ISR(TIMER1_COMPA_vect) {
    time++;
    if(time>=peekMaxDelay) {
        epsonPeek();
    }
}

void loop() {
    if(Serial.available()) {
        serialHandler();
    }
    if(keyboard.available()) {
        ps2Handler();
    }
}

void serialHandler() {
    while(Serial.available()) {
        char c = Serial.read();
        if(c==0x1b) {
            isEscape = 1;
        } else if(isEscape) {
            if(isalpha(c)) {isEscape=0;}
            /*if(escapeType=='[') {if(((int)c>=0x41)&&((int)c<=0x7a)) {isEscape=0;}}
            else {
                if(c=='[') {escapeType=c;} //CSI
                //else if(c=='P') {escapeType=c;} //DCS
                //else if(c==']') {escapeType=c;} //OSC
                else {while(Serial.available()==0) {Serial.read();isEscape=0;}}
            }*/
        } else {
            epsonUnPeek();
            printByte(c);
            DEBUG_PRINT(c);
        }
        //Serial2.print(c);
    }
}

void ps2Handler() {
    char c = keyboard.read();
    if (c==0x7f) { //Delete to backspace
        Serial.print((char)0x08);
        DEBUG_PRINT((char)0x08);
    } else if(c==0x0d) { //Append line feed after a carriage return
        Serial.print((char)0x0d);
        Serial.print((char)0x0a);
        DEBUG_PRINT((char)0x0d);
        DEBUG_PRINT((char)0x0a);
    } else { //Print everything else raw
        Serial.print(c);
        DEBUG_PRINT(c);
    }
    //Serial.print(keyboard.read());
}

void printByte(byte inByte) {
    while(digitalRead(pBusy) == HIGH) {
    // wait for busy to go low
        time = 0;
    }

    int b0 = bitRead(inByte, 0);
    int b1 = bitRead(inByte, 1);
    int b2 = bitRead(inByte, 2);
    int b3 = bitRead(inByte, 3);
    int b4 = bitRead(inByte, 4);
    int b5 = bitRead(inByte, 5);
    int b6 = bitRead(inByte, 6);
    int b7 = bitRead(inByte, 7);

    digitalWrite(pData0, b0);        // set data bit pins
    digitalWrite(pData1, b1);
    digitalWrite(pData2, b2);
    digitalWrite(pData3, b3);
    digitalWrite(pData4, b4);
    digitalWrite(pData5, b5);
    digitalWrite(pData6, b6);
    digitalWrite(pData7, b7);

    digitalWrite(pStrobe, LOW);       // strobe nStrobe to input data bits
    delayMicroseconds(strobeWait);
    digitalWrite(pStrobe, HIGH);

    while(digitalRead(pBusy) == HIGH) {
    // wait for busy line to go low
        time = 0;
    }
}

void printMessage(byte message[]) {
    for(int i = 0;i<strlen(message);i++) {
        printByte(message[i]);
    }
}

void resetPrinter() {
    printByte(27); // reset printer
    printByte('E');
}

void timerSetup() {
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 0xF424;
    TCCR1B = (1<<WGM12) | (1<<CS12);
    TIMSK1 = (1<<OCIE1A);
    sei();
}

void epsonPeek() {
    if(isPeeked==0) {
        isPeeked = 1;
        printMessage("\x1BJ\xD8\x1BJ\xD8\x1BJ\x6C");
    }
    digitalWrite(LED_BUILTIN,isPeeked);
}
void epsonUnPeek() {
    time = 0;
    if(isPeeked) {
        isPeeked = 0;
        printMessage("\x1Bj\xD8\x1Bj\xD8\x1Bj\x6C");
    }
    digitalWrite(LED_BUILTIN,isPeeked);
}

