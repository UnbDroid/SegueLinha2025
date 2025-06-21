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

// Objeto do sensor QTR Direita e esquerda
QTRSensors qtr_D;
QTRSensors qtr_E; 

//Criando Valores para os sensores periféricos
const uint8_t SideSensorCount = 2;
uint16_t LeftValues[SideSensorCount];
uint16_t RightValues[SideSensorCount];

//valores para o mapeamento dos marcos da direita (flags de início e fim)
int cnt=0;
bool marcadores[2] = {false,false};
bool brancoDetectado = false;


int maxSpeed = 255; //
// int minSpeed = 35;  // Velocidade mínima em curvas

// Variáveis do controle PID
int error = 0;
int lastError = 0;
float KP = 1.3;  // força as curvas
float KD = 5;   // Ganho para suavizar as curvas!
float KI = 0;   // Ganho para suavizar as retas! (Não utilizado, efetivamente um PD)


unsigned long startTime;
void setup() {
  {
      Serial.begin(115200);


    MotorD.ligar_motor(0,0);
    MotorE.ligar_motor(0,0);
  
    // Configure the sensors on pins D1 to D8
    qtr.setTypeRC();
    qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);

    qtr_D.setTypeAnalog();
    qtr_D.setSensorPins((const uint8_t[]){D1_D, D2_D}, SideSensorCount);
  
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

    delay(500);

  }


  startTime  = millis(); 
}

void follow_line();
void marcadores_D();
bool faster = false;


void loop(){
  faster = true;
  unsigned long currentTime = millis();
  follow_line();
  // if (!faster && currentTime - startTime >= 20000) faster = true;
  //MotorD.ligar_motor(1,200);
  //MotorE.ligar_motor(1,200);
  //delay(100);
}


void follow_line(){

  marcadores_D();

  uint16_t position = qtr.readLineWhite(sensorValues);

  if (cnt==2){
    MotorD.ligar_motor(1,0);
    MotorE.ligar_motor(1,0);
    delay(10000);
  }

  if (faster) KP = 2.10;

  // Calcula o erro em relação ao centro (3500)
  //error = position - 4500;

  float error = map(position, 0, 7000, -100, 100); 

  int derivative = error - lastError;

  int currentBaseSpeed = 60;

  if (faster) currentBaseSpeed = 110; 
  float normalizedError = constrain(abs(error) / 100.0, 0, 1);  
  
  // Apply quadratic reduction: the larger the error, the more speed is reduced
  float reductionFactor = 1 - (normalizedError * normalizedError);
  currentBaseSpeed *= reductionFactor;
  
  // O 40 é para sinalizar a parada inicial dele
  currentBaseSpeed = constrain(currentBaseSpeed, 40, maxSpeed);
  
  // Correção com PID (PD)
  int correction = KP * error + KD * derivative + KI * ((error + lastError) / 2);
  correction = constrain(correction, -90, 90);
  if (faster) correction = constrain(correction, -150, 150); 

  // Calcula as velocidades dos motores usando a currentBaseSpeed
  int leftSpeed = currentBaseSpeed + (correction);
  int rightSpeed = currentBaseSpeed - (correction);
  if (faster) rightSpeed = currentBaseSpeed - (correction * 5);
  leftSpeed = constrain(leftSpeed, 0, maxSpeed);  // Limita a velocidade do motor esquerdo
  rightSpeed = constrain(rightSpeed, 0, maxSpeed); // Limita a velocidade do motor direito

  // Liga os motores com as velocidades ajustadas
  MotorE.ligar_motor(1, leftSpeed );  // Assumindo que '1' é para frente
  MotorD.ligar_motor(1, rightSpeed); // Assumindo que '1' é para frente

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
  marcadores_D(); // Chama a função para verificar os marcadores à direita
  delay(10); // Pequeno delay para estabilidade e leitura serial*/
}

//função que aumenta os valores do contador quanto vê marcos a direita
void marcadores_D(){

  qtr_D.read(RightValues, QTRReadMode::On);

  if (RightValues[0] < 3400 || RightValues[1]< 3400) {
    brancoDetectado = true;
  }

  else if (brancoDetectado && (RightValues[0] >= 4095 && RightValues[1] >= 4095)) {
    cnt++;
    brancoDetectado = false;
  }

Serial.print("Leitura D1_2: ");
Serial.println(RightValues[0]); // D1_2 está no índice 0
Serial.print("Leitura D2_2: ");
Serial.println(RightValues[1]); // D2_2 está no índice 1


if (!marcadores[cnt]) {
marcadores[cnt] = true;
Serial.print(">> Marca ");
Serial.print(cnt);
Serial.print(" detectada: ");
}

}