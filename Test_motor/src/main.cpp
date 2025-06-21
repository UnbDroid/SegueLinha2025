#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h" 
#include "Pins.h"    


// Objeto do sensor QTR
QTRSensors qtr;

// Objetos dos motores
MotorDC MotorD(ENCA_E , ENCB_E , IN2, IN1); // Motor esquerdo
MotorDC MotorE(ENCA_D , ENCB_D , IN3, IN4); // Motor direito

const uint8_t SensorCount = 8;      // Número de sensores
uint16_t sensorValues[SensorCount]; // Valores dos sensores

int maxSpeed = 255; // Velocidade máxima em linha reta
int minSpeed = 35;  // Velocidade mínima em curvas

// Variáveis do controle PID
int error = 0;
int lastError = 0;
float KP = 1.8;   // 1.3 e base em 80
float KD = 5;   // Ganho para suavizar as curvas!
float KI = 0;   // Ganho para suavizar as retas! (Não utilizado, efetivamente um PD)

/*

Melhores runs:

kp 1.2 kd 5.0 BS= 80


kp 2 kd 10 bs= 100;

*/

void setup() {
  {
    MotorD.ligar_motor(0,0);
    MotorE.ligar_motor(0,0);
  
    // Configure the sensors on pins D1 to D8
    qtr.setTypeRC();
    qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);
  
    Serial.begin(115200);
    //delay(2000);
  
    // Calibrate the sensors
    Serial.println("Calibrating sensors...");
    for (uint16_t i = 0; i < 100; i++)
    {
      qtr.calibrate();
      delay(10);
    }
    Serial.println("Calibration complete.");
  }
}

void follow_line();

void loop(){
  follow_line();
}

void follow_line(){
  uint16_t position = qtr.readLineWhite(sensorValues);

  // Calcula o erro em relação ao centro (3500)
  //error = position - 4500;


  float error = map(position, 3000, 7000, -100, 100); 

  int derivative = error - lastError;

  int currentBaseSpeed = 80;
  float normalizedError = constrain(abs(error) / 100.0, 0, 1);  
  
  // Apply quadratic reduction: the larger the error, the more speed is reduced
  float reductionFactor = 1 - (normalizedError * normalizedError);
  currentBaseSpeed *= reductionFactor;
  
  currentBaseSpeed = constrain(currentBaseSpeed, 40, maxSpeed);
  
  // Correção com PID (PD)
  int correction = KP * error + KD * derivative + KI * ((error + lastError) / 2);
  correction = constrain(correction, -150, 150);


  // Calcula as velocidades dos motores usando a currentBaseSpeed
  int leftSpeed = currentBaseSpeed + (correction);
  int rightSpeed = currentBaseSpeed - (correction);
  leftSpeed = constrain(leftSpeed, 0, 255);  // Limita a velocidade do motor esquerdo
  rightSpeed = constrain(rightSpeed, 0, 255); // Limita a velocidade do motor direito

  // Liga os motores com as velocidades ajustadas
  MotorE.ligar_motor(1, leftSpeed );  // Assumindo que '1' é para frente
  MotorD.ligar_motor(1, rightSpeed ); // Assumindo que '1' é para frente

  // Atualiza o erro anterior
  lastError = error;

  // Imprime dados para depuração
  Serial.print("Current Base Speed: ");
  Serial.print(currentBaseSpeed);
  Serial.print(" error: ");
  Serial.print(error);
  Serial.print(" Pos: ");
  Serial.print(position);
  Serial.print(" | Err: ");
  Serial.print(error);
  Serial.print(" | BaseSpd: "); // Nova informação para depuração
  Serial.print(currentBaseSpeed);
  Serial.print(" | VE: ");
  Serial.print(leftSpeed);
  Serial.print(" | VD: ");
  Serial.println(rightSpeed);

  delay(10); // Pequeno delay para estabilidade e leitura serial*/
}