#include <PS2Keyboard.h>



// parallel port pin# = arduino pin#
const int nStrobe = 2;
const int data_0 = 3;
const int data_1 = 4;
const int data_2 = 5;
const int data_3 = 6;
const int data_4 = 7;
const int data_5 = 8;
const int data_6 = 9;
const int data_7 = 10;
const int nAck = 11;
const int busy = 12;
// keyboard pins
const int ps2Data = 18;
const int ps2IRQ = 19;

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
    Serial.begin(115200);//9600);
    Serial2.begin(115200);//9600);
    keyboard.begin(ps2Data,ps2IRQ);

    pinMode(nStrobe, OUTPUT);      // is active LOW
    digitalWrite(nStrobe, HIGH);   // set HIGH
    pinMode(data_0, OUTPUT);
    pinMode(data_1, OUTPUT);
    pinMode(data_2, OUTPUT);
    pinMode(data_3, OUTPUT);
    pinMode(data_4, OUTPUT);
    pinMode(data_5, OUTPUT);
    pinMode(data_6, OUTPUT);
    pinMode(data_7, OUTPUT);

    pinMode(nAck, INPUT);     // is active LOW
    pinMode(busy, INPUT);

    pinMode(13, OUTPUT);
    pinMode(14, INPUT);   // analog pin 0 on duemilanove and uno
    digitalWrite(14, HIGH); // enable pull-up

    delay(1000);

    timerSetup();

    resetPrinter();
    printMessage(initMessage);

    pinMode(LED_BUILTIN,OUTPUT);

    Serial2.println("Startup complete");
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
    //Add timer for pushing then pulling paper like in the lpsh.go thing
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
            Serial2.print(c);
        }
        //Serial2.print(c);
    }
}

void ps2Handler() {
    char c = keyboard.read();
    if (c==0x7f) { //Delete to backspace
        Serial.print((char)0x08);
        Serial2.print((char)0x08);
    } else if(c==0x0d) { //Append line feed after a carriage return
        Serial.print((char)0x0d);
        Serial.print((char)0x0a);
        Serial2.print((char)0x0d);
        Serial2.print((char)0x0a);
    } else { //Print everything else raw
        Serial.print(c);
        Serial2.print(c);
    }
    //Serial.print(keyboard.read());
}

void printByte(byte inByte) {
    while(digitalRead(busy) == HIGH) {
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

    digitalWrite(data_0, b0);        // set data bit pins
    digitalWrite(data_1, b1);
    digitalWrite(data_2, b2);
    digitalWrite(data_3, b3);
    digitalWrite(data_4, b4);
    digitalWrite(data_5, b5);
    digitalWrite(data_6, b6);
    digitalWrite(data_7, b7);

    digitalWrite(nStrobe, LOW);       // strobe nStrobe to input data bits
    delayMicroseconds(strobeWait);
    digitalWrite(nStrobe, HIGH);

    while(digitalRead(busy) == HIGH) {
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

