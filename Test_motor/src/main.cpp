#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"

// Objeto do sensor QTR
QTRSensors qtr;

// Objetos dos motores
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1); // Motor esquerdo
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4); // Motor direito

const uint8_t SensorCount = 8;      // Número de sensores
uint16_t sensorValues[SensorCount]; // Valores dos sensores

// Variáveis do controle PID
int error = 0;
int lastError = 0;
float KP = 0.01;    // Ganho proporcional
float KD = 0.0;     // Ganho derivativo
int baseSpeed = 50; // Velocidade base dos motores (ajuste conforme necessário)

void setup()
{
  // Liga os LEDs IR dos sensores
  pinMode(LEDON, OUTPUT);
  digitalWrite(LEDON, HIGH);

  // Define os pinos dos sensores
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);

  Serial.begin(9600);
  delay(2000);

  // Calibração dos sensores
  Serial.println("Calibrando sensores...");
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
    delay(10);
  }
  Serial.println("Calibração concluída.");
}

void loop()
{
  // Lê a posição da linha preta (0 a 7000)
  uint16_t position = qtr.readLineBlack(sensorValues);

  // Calcula o erro em relação ao centro (3500)
  error = position - 3500;

  // Derivada do erro
  int derivative = error - lastError;

  // Correção com PID (PD)
  int correction = KP * error + KD * derivative;

  // Calcula as velocidades dos motores
  int leftSpeed = baseSpeed - correction;
  int rightSpeed = baseSpeed + correction;

  // Garante que as velocidades estão dentro do limite 0–255
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  // Liga os motores com as velocidades ajustadas
  MotorE.ligar_motor(1, leftSpeed);
  MotorD.ligar_motor(1, rightSpeed);

  // Atualiza o erro anterior
  lastError = error;

  // Imprime dados para depuração
  Serial.print("Posição: ");
  Serial.print(position);
  Serial.print(" | Erro: ");
  Serial.print(error);
  Serial.print(" | VE: ");
  Serial.print(leftSpeed);
  Serial.print(" | VD: ");
  Serial.println(rightSpeed);

  delay(10);
}
