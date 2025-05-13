#include "LEDClass.h"
#include "AmplifierClass.h"
#include "EncoderClass.h"

// Global flags
bool FLAG_PC_CONNECTED = false;
bool FLAG_SERIAL_OUT_ENABLED = true;
bool FLAG_USE_ENCODER = true;

// LED Flags
bool LED_Connected = false;
bool LED_Enabled = false;

// Amplifier variables
int16_t pwmNewA = 0;
int16_t pwmNewB = 0;
int16_t pwmNewC = 0;

int16_t pwmOldA = 0;
int16_t pwmOldB = 0;
int16_t pwmOldC = 0;

// Declare class objects
LEDClass LEDs;
AmplifierClass Amplifiers;
EncoderClass Encoder;

// Create interval timers
IntervalTimer IT_AMPLIFIER;
IntervalTimer IT_ENCODER;
IntervalTimer IT_SERIAL;

// Interval timer frequencies
uint16_t FREQ_IT_AMPLIFIER = 1000;
uint16_t FREQ_IT_ENCODER = 60;
uint16_t FREQ_IT_SERIAL = 30;

// System state identifier
String SYSTEM_STATE = "WAITING";

// Serial strings
String serialPacketOut = "";
String serialPacketIn = "";



void setup() {

  // Main software serial interface for communications with C++
  Serial.begin(10000000);

  // Second terminal for Teensy to PC feedback
  SerialUSB1.begin(9600);

  // pinMode(0, OUTPUT); WTF is this?

  // Initialize LEDs
  LEDs.Begin();
  // if (FLAG_SERIAL_OUT_ENABLED) SerialUSB1.println("LEDs initialized.");
  delay(500);

  // Initialize Amplifiers
  Amplifiers.Begin();
  // if (FLAG_SERIAL_OUT_ENABLED) SerialUSB1.println("Amplifiers initialized.");
  delay(500);

  // Initialize encoder
  if (FLAG_USE_ENCODER) {
    Encoder.Begin();
    // if (FLAG_SERIAL_OUT_ENABLED) SerialUSB1.println("Encoders initialized!");
    delay(500);
  }

  // Initialize interval timers
  IT_AMPLIFIER.begin(Callback_IT_AMPLIFIER, 1000000 / FREQ_IT_AMPLIFIER);
  if (FLAG_USE_ENCODER) IT_ENCODER.begin(Callback_IT_ENCODER, 1000000 / FREQ_IT_ENCODER);
  IT_SERIAL.begin(Callback_IT_SERIAL, 1000000 / FREQ_IT_SERIAL);

  if (FLAG_SERIAL_OUT_ENABLED) SerialUSB1.println("Teensy ready!");
}


// Main loop
void loop() {

  // Nothing, handled in IntervalTimer callbacks
}




/* CCC */
// Amplifiers.enabled = true;


void Callback_IT_AMPLIFIER() {
  if (Amplifiers.ENABLED) {
    Amplifiers.DrivePWM(pwmNewA, pwmNewB, pwmNewC);
  } else {
    Amplifiers.DrivePWMZero();
  }
}


void Callback_IT_SERIAL() {

  if (FLAG_SERIAL_OUT_ENABLED) {
    BuildOutgoingString();
    SerialUSB1.println(serialPacketOut);
    serialPacketOut = "";
  }
}


void Callback_IT_ENCODER() {

  Encoder.ReadEncoders();
}


/* PPP */

// Function to parse incoming serial packet
void ParseSerialPacket(String newPacket) {

  // Toggle onboard LED
  LEDs.ToggleBuiltInLED();

  char header = newPacket.charAt(2);
  char footer = newPacket.charAt(newPacket.length() - 1);

  // 0         1
  // 012345678901234560
  // E0A0000B0000C0000X

  // Make sure packet is complete
  if (footer == 'X') {

    LEDs.PC(true);

    Amplifiers.ENABLED = bool(newPacket.substring(1, 2).toInt());
    LEDs.Amplifiers(Amplifiers.ENABLED);


    switch (header) {
      case 'A':
        {
          // Parse motor PWM substring
          Serial.print("Parse ");
          ParseAngularError(newPacket);
          // serialPacketOut = newPacket;
        }
    }
  } else {
    LEDs.PC(false);
  }
}





/* SSS */


void serialEvent() {

  serialPacketIn = "";

  while (Serial.available()) {
    digitalWrite(LED_BUILTIN, HIGH);

    char incomingChar = (char)Serial.read();
    if (incomingChar == '\n') {
      // Serial.print("Packet received: ");
      // Serial.println(incomingCommand);
      ParseSerialPacket(serialPacketIn);

    } else {
      serialPacketIn += incomingChar;
    }
  }
}





void ParseAngularError(String newPacket) {

  // Parse telemetry data
  // 0         1
  // 012345678901234567
  // E0A0000B0000C0000X

  // Extract new values
  pwmNewA = newPacket.substring(3, 7).toInt();
  pwmNewB = newPacket.substring(8, 12).toInt();
  pwmNewC = newPacket.substring(13, 17).toInt();

  // Save last value
  pwmOldA = pwmNewA;
  pwmOldB = pwmNewB;
  pwmOldC = pwmNewC;
}


void BuildOutgoingString() {
  // 0123456789
  // TE0A0B0C0X
  int A = map(pwmNewA, 1, 2048, 9, 0);
  int B = map(pwmNewB, 1, 2048, 9, 0);
  int C = map(pwmNewC, 1, 2048, 9, 0);

  serialPacketOut = String("TE") + (Amplifiers.ENABLED ? "1" : "0") + "A" + String(A) + "B" + String(B) + "C" + String(C) + "X";
  Serial.println(serialPacketOut);
}