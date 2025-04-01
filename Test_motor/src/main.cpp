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

void setup() {

    Serial.begin(9600);
    MotorD.configurar(2100, 1.8, 1.3, 0);
    MotorE.configurar(2100, 1.8, 1.3, 0);

   /* qtr.setTypeRC();
    qtr.setSensorPins((const uint8_t[]){32, 33, 25, 26, 27, 14, 12, 13}, SensorCount);

    Serial.println("Calibrating...");
    for (int i = 0; i < 400; i++) {
        qtr.calibrate();
        delay(5);
    }
    Serial.println("Done calibrating.");*/


}

void ligar_motor(int dir, int pwmVal) {
    MotorD.ligar_motor(dir, pwmVal);
    MotorE.ligar_motor(dir, pwmVal);
}

void loop()
{
  /*// read calibrated sensor values and obtain a measure of the line position
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

  delay(250);*/

  MotorD.ligar_motor(1, 255);
  MotorE.ligar_motor(1, 255);
  delay(1000);
  MotorD.ligar_motor(-1, 255);
  MotorE.ligar_motor(-1, 255);
  delay(1000);
  

}
