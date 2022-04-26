#include <PS2Keyboard.h>

#define DEBUG 0 //Will fail to compile if set to 1 and compiling for a board that does not have Serial2
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

//const int lineLen = 80; //Unnessecary until linewrapping is implemented
//int linePos = 0;
const byte initMessage[] = "Initializing...\r\n";

bool isEscape = 0;
bool isPeeked = 0;
unsigned int time = 0;
bool timerInhibit = 0;
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

    DEBUG_PRINTLN("Startup complete");
}

ISR(TIMER1_COMPA_vect) {
    if(!timerInhibit) {
        time++;
        if(time>=peekMaxDelay) {
            epsonPeek();
        }
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
    //Disabled code due to pressing enter twice and keyboard input not actually requiring a linefeed to be appended
    /*} else if(c==0x0d) { //Append line feed after a carriage return
        Serial.print((char)0x0d);
        Serial.print((char)0x0a);
        DEBUG_PRINT((char)0x0d);
        DEBUG_PRINT((char)0x0a);*/
    } else { //Print everything else raw
        Serial.print(c);
        DEBUG_PRINT(c);
    }
    //Serial.print(keyboard.read());
}

void printByte(byte inByte) {
    timerEnable(0);
    while(digitalRead(pBusy) == HIGH) {
    // wait for busy to go low
    }

    digitalWrite(pData0, inByte & 0x01);        // set data bit pins
    digitalWrite(pData1, inByte & 0x02);        // set data bit pins
    digitalWrite(pData2, inByte & 0x04);        // set data bit pins
    digitalWrite(pData3, inByte & 0x08);        // set data bit pins
    digitalWrite(pData4, inByte & 0x10);        // set data bit pins
    digitalWrite(pData5, inByte & 0x20);        // set data bit pins
    digitalWrite(pData6, inByte & 0x40);        // set data bit pins
    digitalWrite(pData7, inByte & 0x80);        // set data bit pins

    digitalWrite(pStrobe, LOW);       // strobe pStrobe to input data bits
    delayMicroseconds(strobeWait);
    digitalWrite(pStrobe, HIGH);
    timerEnable(1);
}

void printMessage(byte message[]) {
    for(int i = 0;i<strlen(message);i++) {
        printByte(message[i]);
    }
}

void resetPrinter() {
    printMessage("\x1B@"); // Initialize printer
    printMessage("\x1Bx0"); // Set printing quality to DRAFT
    printMessage("\x1B!\x04"); // Enable condensed printing
}

void timerSetup() {
    timerEnable(0);
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 0xF424;
    TCCR1B = (1<<WGM12) | (1<<CS12);
    TIMSK1 = (1<<OCIE1A);
    sei();
}

void timerEnable(bool enable) { // Wrapper function to set timerInhibit to x and time to 0
    if(enable==1) {
        timerInhibit = 0;
    } else {
        timerInhibit = 1;
    }
    time = 0;
}

/* Functions to move the page up if nothing has been printed for x amount of time
to allow the user to read the output without the print head covering the page
Then moving the page back down when text needs to be printed again
Note: Uses Espon LQ1070/570 Control Codes*/
void epsonPeek() {
    if(isPeeked==0) {
        isPeeked = 1;
        printMessage("\x1BJ\xD8\x1BJ\xD8\x1BJ\x6C");
    }
}
void epsonUnPeek() {
    time = 0;
    if(isPeeked) {
        isPeeked = 0;
        printMessage("\x1Bj\xD8\x1Bj\xD8\x1Bj\x6C");
    }
}

