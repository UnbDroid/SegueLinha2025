#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"

// Criação dos objetos Motor esquerdo e direito
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1);
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4);


// Criação dos objetos QTRSensors
QTRSensors qtr;

/*unsigned long pos1 = 0; // Variável para armazenar a posição do encoder do motor 1
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

*/


const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

void setup()
{
  // configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6,D7,D8}, SensorCount);

  delay(500);


  // analogRead() takes about 0.1 ms on an AVR.
  // 0.1 ms per sensor * 4 samples per sensor read (default) * 6 sensors
  // * 10 reads per calibrate() call = ~24 ms per calibrate() call.
  // Call calibrate() 400 times to make calibration take about 10 seconds.
  Serial.begin(9600);

  delay(2000);
  Serial.println("QTR Calibration");

  pinMode(LEDON, OUTPUT);
  digitalWrite(LEDON, HIGH); // Turn on the LED
  Serial.println("Calibrating... (this will take about 10 seconds)");
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  Serial.println("Done calibrating.");

  // print the calibration minimum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);
}

void loop()
{
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);

  if (sensorValues[1] > 800 && sensorValues[2] > 800 && sensorValues[3] > 800 && sensorValues[4] > 800 && sensorValues[5] > 800 && sensorValues[6] > 800 && sensorValues[7] > 800){
    MotorD.ligar_motor(1,100);
    MotorE.ligar_motor(1,100);
  }
  else if(sensorValues[0] < 500){
    MotorD.ligar_motor(-1,100);
    MotorE.ligar_motor(1,100);
  }
  else if(sensorValues[7] < 500){
    MotorD.ligar_motor(1,100);
    MotorE.ligar_motor(-1,100);
  }

  delay(250);
}
