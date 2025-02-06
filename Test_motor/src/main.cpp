#include <Arduino.h>

#define ENCA1 3
#define ENCB1 26
#define ENA 5
#define IN2 42
#define IN1 43

#define ENCA2 2
#define ENCB2 28
#define ENB 7
#define IN4 48
#define IN3 49

unsigned long pos1 = 0; // Variável para armazenar a posição do encoder do motor 1
unsigned long pos2 = 0; // Variável para armazenar a posição do encoder do motor 2

// Declaração das funções
void readencoder1();
void readencoder2();
void setMotor(int dir, int pwmVal, int pwm, int in1, int in2);

void setup() {
Serial.begin(9600);
// Configuração do motor 1
pinMode(ENCA1, INPUT);
pinMode(ENCB1, INPUT);
pinMode(IN2, OUTPUT);
pinMode(IN1, OUTPUT);
pinMode(ENA, OUTPUT);
attachInterrupt(digitalPinToInterrupt(ENCA1), readencoder1, RISING);


// Configuração do motor 2
pinMode(ENCA2, INPUT);
pinMode(ENCB2, INPUT);
pinMode(IN4, OUTPUT);
pinMode(IN3, OUTPUT);
pinMode(ENB, OUTPUT);
attachInterrupt(digitalPinToInterrupt(ENCA2), readencoder2, RISING);
 
}

void loop() {
Serial.println(pos1);
Serial.println(pos2);
setMotor(1, 255, ENA, IN1, IN2); 
setMotor(1, 255, ENB, IN3, IN4); 
delay(1000);
setMotor(0, 0, ENA, IN1, IN2); // Parar o motor 1
setMotor(0, 0, ENB, IN3, IN4); // Parar o motor 2
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