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
  
  Serial.begin(115200);
  delay(2000);
  

  // Configure QTR sensor
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);
  
  // Hardcoded calibration (replace with your actual values)
  const uint16_t minValues[SensorCount] = {345, 369, 266, 203, 264, 263, 370, 370};
  const uint16_t maxValues[SensorCount] = {449, 2500, 2500, 2500, 2500, 2500, 2500};
  
  qtr.calibrationOn.minimum = new uint16_t[SensorCount];
  qtr.calibrationOn.maximum = new uint16_t[SensorCount];
  qtr.calibrationOn.initialized = true;
  std::copy(minValues, minValues + 8, qtr.calibrationOn.minimum);
  std::copy(maxValues, maxValues + 8, qtr.calibrationOn.maximum);

}

void calibration_values();
void follow_line();

void loop(){

  //calibration_values();
  // Por a função de testes e "follow line" para partida.
  //follow_line();
  
  Serial.end(); 
  

}


void calibration_values() {
  int k = 1;
  while(k){
    int valoresSensoresMin[8];
    int valoresSensoresMax[8];

    for (uint16_t i = 0; i < 400; i++) {
      qtr.calibrate();
      delay(10);
    }

    for (uint16_t i = 0; i < SensorCount; i++) {
      valoresSensoresMin[i] = qtr.calibrationOn.minimum[i];
      valoresSensoresMax[i] = qtr.calibrationOn.maximum[i];
    }

    // Print minimum values
    Serial.println("Min values:");
    for (uint16_t i = 0; i < SensorCount - 1; i++) {
      Serial.print(valoresSensoresMin[i]);
      Serial.print(", ");
    }
  
    Serial.println(valoresSensoresMin[SensorCount - 1]);

    // Print maximum values
    Serial.println("Max values:");
    for (uint16_t i = 0; i < SensorCount - 1; i++) {
      Serial.print(valoresSensoresMax[i]);
      Serial.print(", ");

    }
    Serial.println(valoresSensoresMax[SensorCount - 1]);
  }

  Serial.end();

  


}



void follow_line(){
  uint16_t position = qtr.readLineWhite(sensorValues);

  // Calcula o erro em relação ao centro (3500)
  int error = map(position, 0, 7000, -100, 100);  

  int derivative = error - lastError;

  int currentBaseSpeed = 75;
  float normalizedError = constrain(abs(error) / 100.0, 0, 1);  
  
  // Apply quadratic reduction: the larger the error, the more speed is reduced
  float reductionFactor = 1 - (normalizedError * normalizedError);
  currentBaseSpeed *= reductionFactor;
  
  currentBaseSpeed = constrain(currentBaseSpeed, 40, maxSpeed);
  
  // Correção com PID (PD)
  int correction = KP * error + KD * derivative + KI * ((error + lastError) / 2);


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