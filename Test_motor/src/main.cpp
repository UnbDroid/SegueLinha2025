#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"
#include "algorithm"

// QTRSensors object
QTRSensors qtr;

// Motor objects
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1);
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4); 
int maxRPM = 1100; // Maximum RPM for the motors
int minRPM = 216; // Minimum RPM for the motors TODO: test this with the motor loaded

const uint8_t SensorCount = 8; // Number of sensors
uint16_t sensorValues[SensorCount]; // Array to store sensor values

void encoder_callback() {
  MotorD.ler_encoder();
}

void setup()
{ 
  Serial.begin(115200);
  delay(100);
  
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);
  
  //Hardcoded sensor values 
  const uint16_t minValues[SensorCount] = {100, 110, 120, 130, 140, 150, 160, 170};
  const uint16_t maxValues[SensorCount] = {900, 890, 880, 870, 860, 850, 840, 830};
  
  qtr.calibrationOn.minimum = new uint16_t[SensorCount];
  qtr.calibrationOn.maximum = new uint16_t[SensorCount];

  qtr.calibrationOn.initialized = true;
  std::copy(minValues, minValues + 8, qtr.calibrationOn.minimum);
  std::copy(maxValues, maxValues + 8, qtr.calibrationOn.maximum);
  
  attachInterrupt(digitalPinToInterrupt(ENCB1), encoder_callback, RISING);

  MotorD.configurar(12.0, 1.0, 0.0, 0.0); // Configure MotorD with ticks per revolution and PID constants
  MotorE.configurar(12.0, 1.0, 0.0, 0.0); // Configure MotorE with ticks per revolution and PID constants

  MotorD.ligar_motor(0,0);
  MotorE.ligar_motor(0,0);
}

void MOTORPID_TEST(int desiredSpeed, float kp, float kd, float ki){
  Serial.println("Motor PID test");
  // Modificar apenas KP, KD  e KI. 
  // Não modificar o valor de ticks por volta, pois isso é calculado com base no encoder do motor.
  MotorE.configurar(12.0, kp, kd, ki);
  MotorD.configurar(12.0, kp, kd, ki);

  MotorD.set_RPM(desiredSpeed);
  MotorE.set_RPM(desiredSpeed);
  delay(100);

  Serial.print("Desidered RPM: " + desiredSpeed);
  Serial.print(" Motor E RPM: " + String(MotorE.get_rpm()));
  Serial.println(" Motor D RPM: " + String(MotorD.get_rpm()) + "\n");

}

int16_t prevError = 0;
float integral = 0;
void TEST_SENSOR(int baseRPM, float kp, float kd, float ki){
  uint16_t position = qtr.readLineWhite(sensorValues);
  int16_t error = position - (SensorCount - 1) / 2; 

  integral += error;
  int16_t derivative = error - prevError;

  int correction = kp * error + ki * integral + kd * derivative;
  prevError = error;

  int rpmLeft  = constrain(baseRPM - correction, minRPM, maxRPM);
  int rpmRight = constrain(baseRPM + correction, minRPM, maxRPM);

  MotorD.set_RPM(rpmRight);
  MotorE.set_RPM(rpmLeft);
  Serial.print("Left RPM: " + String(rpmLeft));
  Serial.print(" Right RPM: " + String(rpmRight));
  Serial.print(" Error: " + String(error));
  Serial.println(" Correction: " + String(correction));
}


void loop(){
  
}
