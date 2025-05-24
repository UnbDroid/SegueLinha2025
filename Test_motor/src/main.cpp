#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h" // Certifique-se que esta biblioteca e a MotorDC.cpp estão no seu projeto
#include "Pins.h"    // Certifique-se que este arquivo com as definições dos pinos está no seu projeto

// Objeto do sensor QTR
QTRSensors qtr;

// Objetos dos motores
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1); // Motor esquerdo
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4); // Motor direito

const uint8_t SensorCount = 8;      // Número de sensores
uint16_t sensorValues[SensorCount]; // Valores dos sensores

// --- Novas Constantes para Controle de Velocidade Dinâmico ---
const int MAX_SPEED_STRAIGHT = 100;   // Velocidade máxima em retas (respeitando o limite original)
const int MIN_SPEED_CURVE = 35;       // Velocidade mínima em curvas (ajuste conforme necessário)
const int ERROR_THRESHOLD_MAX_REDUCTION = 50; // 2000 Magnitude do erro a partir da qual a velocidade estará no mínimo 
                                                // (erro máximo é ~3500, então 2000 é uma curva já considerável)
                                                // Ajuste este valor experimentalmente.

// Variáveis do controle PID
int error = 0;
int lastError = 0;
float KP = 2;   // Ganho pra curvas! 5.8
float KD = 2.8;   // Ganho para suavizar as curvas!
float KI = 0.0;   // Ganho para suavizar as retas! (Não utilizado, efetivamente um PD)
// int baseSpeed = 100; // Esta será substituída pela velocidade dinâmica

void setup()
{
  // Liga os LEDs IR dos sensores
  pinMode(LEDON, OUTPUT);
  digitalWrite(LEDON, HIGH);

  // Define os pinos dos sensores
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);

  Serial.begin(115200);
  delay(2000); // Espera para abrir o Monitor Serial, se necessário

  // Calibração dos sensores
  Serial.println("Calibrando sensores...");
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
    delay(10);
  }
  Serial.println("Calibração concluída.");
  Serial.println("------------------------------------");
  Serial.println("Iniciando loop principal...");
}

void loop()
{
  // Lê a posição da linha preta (0 a 7000 para 8 sensores)
  uint16_t position = qtr.readLineBlack(sensorValues);

  // Calcula o erro em relação ao centro (3500)
  error = position - 3500;

  // Derivada do erro
  int derivative = error - lastError;

  // --- Ajuste Dinâmico da Velocidade Base ---
  int currentBaseSpeed;
  long errorMagnitude = abs(error); // Usamos long para a função map

  // Mapeia a magnitude do erro para a faixa de velocidade desejada
  // Se errorMagnitude = 0, currentBaseSpeed = MAX_SPEED_STRAIGHT
  // Se errorMagnitude >= ERROR_THRESHOLD_MAX_REDUCTION, currentBaseSpeed = MIN_SPEED_CURVE
  currentBaseSpeed = map(errorMagnitude, 0, ERROR_THRESHOLD_MAX_REDUCTION, MAX_SPEED_STRAIGHT, MIN_SPEED_CURVE);
  
  // Garante que a currentBaseSpeed não saia dos limites definidos
  currentBaseSpeed = constrain(currentBaseSpeed, MIN_SPEED_CURVE, MAX_SPEED_STRAIGHT);

  // Correção com PID (PD)
  int correction = KP * error + KD * derivative; // + KI * (error + lastError) / 2;

  // Calcula as velocidades dos motores usando a currentBaseSpeed
  int leftSpeed = currentBaseSpeed + correction;
  int rightSpeed = currentBaseSpeed - correction;

  // Garante que as velocidades estão dentro do limite operacional global (0–110, como no original)
  // É importante que MAX_SPEED_STRAIGHT não seja maior que o limite aqui,
  // e que MIN_SPEED_CURVE permita que o robô ainda se mova e corrija.
  leftSpeed = constrain(leftSpeed, 0, MAX_SPEED_STRAIGHT); // Usando MAX_SPEED_STRAIGHT como limite superior prático
  rightSpeed = constrain(rightSpeed, 0, MAX_SPEED_STRAIGHT);

  // Liga os motores com as velocidades ajustadas
  MotorE.ligar_motor(1, leftSpeed);  // Assumindo que '1' é para frente
  MotorD.ligar_motor(1, rightSpeed); // Assumindo que '1' é para frente

  // Atualiza o erro anterior
  lastError = error;

  // Imprime dados para depuração
  Serial.print("Pos: ");
  Serial.print(position);
  Serial.print(" | Err: ");
  Serial.print(error);
  Serial.print(" | BaseSpd: "); // Nova informação para depuração
  Serial.print(currentBaseSpeed);
  Serial.print(" | VE: ");
  Serial.print(leftSpeed);
  Serial.print(" | VD: ");
  Serial.println(rightSpeed);

  delay(10); // Pequeno delay para estabilidade e leitura serial
}