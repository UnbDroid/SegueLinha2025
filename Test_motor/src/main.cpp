#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"

// Criação dos objetos Motor esquerdo e direito
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1);
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4);


// Criação dos objetos QTRSensors
QTRSensors qtr;

unsigned long pos1 = 0; // Variável para armazenar a posição do encoder do motor 1
unsigned long pos2 = 0; // Variável para armazenar a posição do encoder do motor 2



const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
#define encoderA 3  // Yellow wire#define encoderA 2  // Yellow wire
#define encoderB 2  // Green wire

volatile int position = 0; // Keeps track of the encoder position
int lastAState;

// Interrupt Service Routine (ISR) to read encoder changes
void readEncoder() {
  int A = digitalRead(encoderA);
  int B = digitalRead(encoderB);
  
  if (A != lastAState) { // If state of A has changed
    if (B != A) { 
      position++; // Clockwise
    } else {
      position--; // Counterclockwise
    }
  }
  lastAState = A;
}
void setup() {
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  
  // Attach an interrupt to encoderA (pin 2), triggering on state change
  attachInterrupt(digitalPinToInterrupt(encoderA), readEncoder, CHANGE);

  Serial.begin(9600);
  lastAState = digitalRead(encoderA);
}

void loop() {
  Serial.println(position); // Print encoder position
  MotorD.ligar_motor(1,100);
  MotorE.ligar_motor(-1,255);
  delay(100);


}

