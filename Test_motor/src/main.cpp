#include <Arduino.h>

#define ENCA 2
#define ENCB 7
#define R_EN 5
#define RPWM 38
#define LPWM 42

unsigned long pos = 0; // Variável para armazenar a posição do encoder

// Declaração das funções
void readencoder();
void setMotor(int dir, int pwmVal, int pwm, int in1, int in2);

void setup() {
  Serial.begin(9600);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), readencoder, RISING);
  setMotor(1, 255, R_EN, LPWM, RPWM); 
}

void loop() {
  //Serial.println(pos);
  //setMotor(1, 255, R_EN, LPWM, RPWM); // Mover o motor para frente com velocidade máxima
  //delay(1000);
  //setMotor(0, 0, RPWM, LPWM, RPWM); // Parar o motor
  //delay(1000);
}

void readencoder() {
  int b = digitalRead(ENCA);
  if (b > 0) {
    pos++;
  } else {
    pos--;
  }
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2) {
  analogWrite(pwm, pwmVal);
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
//10000