#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"

// Criação dos objetos Motor esquerdo e direito
MotorDC MotorE(ENCA1, ENCB1, ENA, IN2, IN1);
MotorDC MotorD(ENCA2, ENCB2, ENB, IN3, IN4);

unsigned long pos1 = 0; // Variável para armazenar a posição do encoder do motor 1
unsigned long pos2 = 0; // Variável para armazenar a posição do encoder do motor 2

// Declaração das funções
void readencoder1();
void readencoder2();


void setup() {
    Serial.begin(9600);
    attachInterrupt(digitalPinToInterrupt(ENCA1), readencoder1, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCA2), readencoder2, RISING);

    MotorD.configurar(2100, 1.8, 1.3, 0);
    MotorE.configurar(2100, 1.8, 1.3, 0);

}

void loop() {
    Serial.println(pos1);
    Serial.println(pos2);
    MotorD.ligar_motor(1,200);
    MotorE.ligar_motor(1,200);
    delay(1000);

    MotorD.parar();
    MotorE.parar();
    delay(1000);


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