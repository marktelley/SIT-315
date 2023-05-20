/*
 *   Copyright (c) 2022 Author: MARK TELLEY.
 *   All rights reserved.
 */

//---------
// VARIABLES
//---------
// init
const int switchRIGHT_pin = 12;
const int switchLEFT_pin = 11;
const int btn_pin = 2;
const int motion_pin = 3;
const int LEDGREEN_pin = 8;    // Green
const int LEDRED_pin = 9;      // Red
const int LEDBLUE_pin = 10;    // Blue
const int LEDYELLOW_pin = 7;   // Yellow
const int TIMERWHITE_pin = 13; // White
const String interrupt = "interrupt!";
// Globals
String Status = "Off";
int motion = 0;     // motion detected value
int GREEN = 0;      // pushbutton LED -
int RED = 0;        // swicth 1 LED -
int BLUE = 0;       // switch 2 LED -
int YELLOW = 0;     // motion sensor LED -
int WHITETIMER = 0; // timer interrupt LED

void loop()
{
}

//---------
// SETUP
//---------
void setup()
{
    Serial.begin(9600);
    pins();  // set up pins
    cli();   // Disable interrupt
    timer(); // Set up Timer
    sei();   // Enable interrupt
             // Begin
    interruptsfunc();
}

//---------
// SET UP FUNCTIONS
//---------

void pins()
{
    // Sensors
    pinMode(btn_pin, INPUT);
    pinMode(switchRIGHT_pin, INPUT);
    pinMode(switchLEFT_pin, INPUT);
    pinMode(motion_pin, INPUT);
    // LED
    pinMode(LEDGREEN_pin, OUTPUT);
    pinMode(LEDRED_pin, OUTPUT);
    pinMode(LEDBLUE_pin, OUTPUT);
    pinMode(LEDYELLOW_pin, OUTPUT);
    pinMode(TIMERWHITE_pin, OUTPUT);
}

void timer()
{
    // TIMER
    TCCR1A = 0;
    TCCR1B = 0;
    // Prescaler 256
    TCCR1B |= (1 << CS12);
    TCCR1B &= ~(1 << CS11);
    TCCR1B &= ~(1 << CS10);
    // Compare (https://forum.arduino.cc/t/timer-fundamental-diff-between-ocr1a-and-ocr1b/202188)
    TCNT1 = 0;                              // zero
    OCR1A = 16000000 / ((62499 + 1) * 256); // Set Comparer
    TIMSK1 |= (1 << OCIE1A);                // Output Compare Match A Interrupt Enable
    // Pin interrupts
    PCICR |= 0b00000001;  // enable PCMSK0 to interupt (pins 8-13)
    PCMSK0 |= 0b00011000; // enable pin 11 and 12 to interupt
}

void interruptsfunc()
{
    // Button Interrupt
    attachInterrupt(digitalPinToInterrupt(btn_pin), button_ISR, RISING);
    // Motion Interrupt
    attachInterrupt(digitalPinToInterrupt(motion_pin), motion_ISR, CHANGE);
}

// Timer Interrupt
ISR(TIMER1_COMPA_vect)
{
    // Timer interrupt to console
    Serial.println("timer interrupt!");
    WHITETIMER = not WHITETIMER; // back to off
    digitalWrite(TIMERWHITE_pin, WHITETIMER);
}

// Switches (left and right) interrupt PORT B pins 8-13
ISR(PCINT0_vect)
{
    Serial.println(interrupt);
    // check which pin has changed and then change LED status of that pin
    if (digitalRead(switchRIGHT_pin) != RED)
    {
        RED = not RED;
        digitalWrite(LEDRED_pin, RED);
        switched_output(RED); // Print
    }
    else if (digitalRead(switchLEFT_pin) != BLUE)
    {
        BLUE = not BLUE;
        digitalWrite(LEDBLUE_pin, BLUE);
        switched_output(BLUE); // print
    }
    else
        Serial.println("issue with switches");
}

//---------
// PROGRAM FUNCTIONS
//---------

// button interrupt
void button_ISR()
{
    Serial.println(interrupt);
    GREEN = not GREEN;
    digitalWrite(LEDGREEN_pin, GREEN);
    button_output(); // print
}

// motion sensor interrupt
void motion_ISR()
{
    Serial.println(interrupt);
    motion = digitalRead(motion_pin); // Get motion info
    YELLOW = not YELLOW;              // Set again
    digitalWrite(LEDYELLOW_pin, YELLOW);
    motion_output(); // print
}

String offOn(int stat)
{
    if (stat >= 1)
        Status = "On";
    else
        Status = "Off";
    return Status;
}

// Button
void button_output()
{
    Serial.println("Button pressed|| LED: " + offOn(GREEN));
}

// Switch
void switched_output(int LED)
{
    Serial.println("Switch Changed || LED: " + offOn(LED));
}

// output motion sensor data
void motion_output()
{
    if (motion == 1)
        Serial.println("Motion detected");
    else
        Serial.println("No motion");
    Serial.println("LED: " + offOn(YELLOW));
}

//---------
// REFERENCES
//---------

// Ref
// https://maker.pro/arduino/projects/timer-interrupts-improve-your-arduino-programming-skills
// https://www.arduino.cc/reference/en/language/functions/interrupts/interrupts/
// https://thewanderingengineer.com/2014/08/11/arduino-pin-change-interrupts/
// https://create.arduino.cc/projecthub/rafitc/interrupts-basics-f475d5
// https://www.arduino.cc/reference/en/language/functions/interrupts/nointerrupts/
// https://thewanderingengineer.com/2014/08/11/arduino-pin-change-interrupts/
// https://circuitdigest.com/microcontroller-projects/arduino-timer-tutorial
// https://www.instructables.com/Arduino-Timer-Interrupts/
