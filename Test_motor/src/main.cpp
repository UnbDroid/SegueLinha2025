#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"

// Criação dos objetos Motor esquerdo e direito
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1);
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4);


// Criação dos objetos QTRSensors
QTRSensors qtr;
int error = 0;
int m1Speed = 0;
int m2Speed = 0;
int motorSpeed = 0;
int lastError = 0;
float KP = 0;
float KD = 0;
int M1 = 50; //base motor speeds
int M2 = 50; //base motor speeds

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

  // compute our "error" from the line position
  // error is zero when the middle sensor is over the line,
  error = position - 3500; /// using an 8 sensor array targeting middle of position 4 to 5

  // set the motor speed based on proportional and derivative PID terms
  motorSpeed = KP * error + KD * (error - lastError);
  lastError = error;
  /////////CHANGE THE - OR + BELOW DEPENDING ON HOW YOUR DRIVER WORKS
  m1Speed = M1 - motorSpeed; // M1 and M2 are base motor speeds.
  m2Speed = M2 + motorSpeed;

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
    MotorD.ligar_motor(1,m1Speed);
    MotorE.ligar_motor(1,m2Speed);
  }
  else if(sensorValues[0] < 500){
    MotorD.ligar_motor(-1,m1Speed);
    MotorE.ligar_motor(1,m2Speed);
  }
  else if(sensorValues[7] < 500){
    MotorD.ligar_motor(1,m1Speed);
    MotorE.ligar_motor(-1,m2Speed);
  }

  delay(250);
}

/*void forward()
{
  TimeCheck();
  motorDrive(motor1, turnCW, m1Speed);
  motorDrive(motor2, turnCW, m2Speed);
}

/* //Apply Brakes, then into Standby
  motorBrake(motor1);
  motorBrake(motor2);
  motorsStandby(); 

void TimeCheck() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > BlinkTime) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    BlinkIt();
  }
}

void motorDrive(boolean motorNumber, boolean motorDirection, int motorSpeed)
{
  /*
    This Drives a specified motor, in a specific direction, at a specified speed:
    - motorNumber: motor1 or motor2 ---> Motor 1 or Motor 2
    - motorDirection: turnCW or turnCCW ---> clockwise or counter-clockwise
    - motorSpeed: 0 to 255 ---> 0 = stop / 255 = fast
  

  boolean pinIn1; //Relates to AIN1 or BIN1 (depending on the motor number specified)

  //Specify the Direction to turn the motor
  //Clockwise: AIN1/BIN1 = HIGH and AIN2/BIN2 = LOW
  //Counter-Clockwise: AIN1/BIN1 = LOW and AIN2/BIN2 = HIGH
  if (motorDirection == turnCW)
    pinIn1 = HIGH;
  else
    pinIn1 = LOW;

  //Select the motor to turn, and set the direction and the speed
  if (motorNumber == motor1)
  {
    digitalWrite(pinAIN1, pinIn1);
    digitalWrite(pinAIN2, !pinIn1); //This is the opposite of the AIN1
    analogWrite(pinPWMA, motorSpeed);
  }
  else
  {
    digitalWrite(pinBIN1, pinIn1);
    digitalWrite(pinBIN2, !pinIn1); //This is the opposite of the BIN1
    analogWrite(pinPWMB, motorSpeed);
  }

  //Finally , make sure STBY is disabled - pull it HIGH
  digitalWrite(pinSTBY, HIGH);
}

void motorBrake(boolean motorNumber)
{
  /*
    This "Short Brake"s the specified motor, by setting speed to zero
  

  if (motorNumber == motor1)
    analogWrite(pinPWMA, 0);
  else
    analogWrite(pinPWMB, 0);
}

void motorStop(boolean motorNumber)
{
  /*
    This stops the specified motor by setting both IN pins to LOW
  
  if (motorNumber == motor1) {
    digitalWrite(pinAIN1, LOW);
    digitalWrite(pinAIN2, LOW);
  }
  else
  {
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, LOW);
  }
}

void BlinkIt() {
  if (BlinkCycle == 1) {
    //Led On
    digitalWrite(IndicatorLED, HIGH);
  }
  if (BlinkCycle == 2) {
    //Led Off
    digitalWrite(IndicatorLED, LOW);
    BlinkCycle = 0; // reset BlinkCycle
  }
  BlinkCycle++; // Set BlinkCycle to second mode
}*/