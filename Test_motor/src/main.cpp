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

// Declaração das funções
void readencoder1();
void readencoder2();

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

void setup() {

    Serial.begin(9600);
    attachInterrupt(digitalPinToInterrupt(ENCA1), readencoder1, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCA2), readencoder2, RISING);

    //MotorD.configurar(2100, 1.8, 1.3, 0);
    //MotorE.configurar(2100, 1.8, 1.3, 0);

   /* qtr.setTypeRC();
    qtr.setSensorPins((const uint8_t[]){32, 33, 25, 26, 27, 14, 12, 13}, SensorCount);

    Serial.println("Calibrating...");
    for (int i = 0; i < 400; i++) {
        qtr.calibrate();
        delay(5);
    }
    Serial.println("Done calibrating.");*/


}



void ligar_motor(int direcao, int pwmVal){

  if (direcao == 1){
    // 1 para frente
    analogWrite(IN1, pwmVal);
    analogWrite(IN2, 0);
    analogWrite(IN3, pwmVal);
    analogWrite(IN4, 0);
  }
  else if (direcao == -1){
    // -1 para trás
    analogWrite(IN1, 0);
    analogWrite(IN2, pwmVal);
    analogWrite(IN3, 0);
    analogWrite(IN4, pwmVal);
  }
  else{ // 0 para parar
    digitalWrite(IN1, 255);
    analogWrite(IN2, 255);
    digitalWrite(IN3, 255);
    analogWrite(IN4, 255);
  }

}

void ligar_motor2(int direcao, int pwmVal){

  if (direcao == 1){
    // 1 para frente
    analogWrite(IN3, pwmVal);
    analogWrite(IN4, 0);
  }
  else if (direcao == -1){
    // -1 para trás
    analogWrite(IN3, 0);
    analogWrite(IN4, pwmVal);
  }
  else{ // 0 para parar
    digitalWrite(IN3, 255);
    analogWrite(IN4, 255);
  }

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

  ligar_motor(1,200);

}

void readencoder1() {
int b = digitalRead(ENCA1);
if (b > 0) {
pos1++;
} else {
pos1--;
}
}

void readencoder2() {
int b = digitalRead(ENCA2);
if (b > 0) {
pos2++;
} else {
pos2--;
}
}

void setMotor(int dir, int pwmVal, int en, int in1, int in2) {
analogWrite(en, pwmVal);
if (dir == 1) {
digitalWrite(in1, HIGH);
digitalWrite(in2, LOW);
} else if (dir == -1) {
digitalWrite(in1, LOW);
digitalWrite(in2, HIGH);
} else {
digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
}
}