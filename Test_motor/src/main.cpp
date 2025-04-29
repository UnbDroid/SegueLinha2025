#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"

// QTRSensors object
QTRSensors qtr;

// Motor objects
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1); // Left motor
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4); // Right motor

const uint8_t SensorCount = 8; // Number of sensors
uint16_t sensorValues[SensorCount]; // Array to store sensor values


void setup()
{
  // Configure the LEDON pin
  pinMode(LEDON, OUTPUT);
  digitalWrite(LEDON, HIGH); // Turn on the IR LEDs

  // Configure the sensors on pins D1 to D8
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);

  Serial.begin(9600);
  delay(2000);

  // Calibrate the sensors
  Serial.println("Calibrating sensors...");
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
    delay(10);
  }
  Serial.println("Calibration complete.");
}
// PID control variables
int error = 0;
int lastError = 0; // To store the previous error
float KP = 0.1;    // Proportional gain
float KD = 0.05;   // Derivative gain
int baseSpeed = 100; // Base motor speed

void loop()
{
  // Read the line position (0 to 7000 for 8 sensors)
  uint16_t position = qtr.readLineBlack(sensorValues);

  // Calculate the error (center is 3500 for 8 sensors)
  error = position - 3500;

  // Calculate motor speed adjustments using proportional and derivative control
  int motorSpeed = KP * error + KD * (error - lastError);

  // Update lastError for the next iteration
  lastError = error;

  // Set motor speeds
  int leftMotorSpeed = baseSpeed - motorSpeed;
  int rightMotorSpeed = baseSpeed + motorSpeed;

  // Constrain motor speeds to valid range (0 to 255)
  leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);
  rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);

  // Drive the motors
  MotorE.ligar_motor(1, leftMotorSpeed);
  MotorD.ligar_motor(1, rightMotorSpeed);

  // Optional: Print debug information
  Serial.print("Position: ");
  Serial.print(position);
  Serial.print(" Error: ");
  Serial.print(error);
  Serial.print(" Left Speed: ");
  Serial.print(leftMotorSpeed);
  Serial.print(" Right Speed: ");
  Serial.println(rightMotorSpeed);

  delay(10); // Small delay for stability
}