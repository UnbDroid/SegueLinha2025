#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h" // Certifique-se que esta biblioteca e a MotorDC.cpp estão no seu projeto
#include "Pins.h"    // Certifique-se que este arquivo com as definições dos pinos está no seu projeto

// Objeto do sensor QTR
QTRSensors qtr;
QTRSensors Perifericos;

// Objetos dos motores
//MotorDC MotorE(ENCA_E , ENCB_E , IN2, IN1); // Motor esquerdo
//MotorDC MotorD(ENCA_D , ENCB_D , IN3, IN4); // Motor direito

const uint8_t SensorCount1 = 8;      // Número de sensores no array principal
uint16_t sensorValues1[SensorCount1]; // Valores dos sensores no array principal

const uint8_t SensorCount2 = 2;      // Número de sensores no array periférico
uint16_t sensorValues2[SensorCount2]; // Valores dos sensores no array periférico

int maxSpeed = 255; // Velocidade máxima em linha reta
int minSpeed = 35;  // Velocidade mínima em curvas

// Variáveis do controle PID
int error = 0;
int lastError = 0;
float KP = 1.2;   // 1.3 e base em 80
float KD = 3;   // Ganho para suavizar as curvas!
float KI = 0;   // Ganho para suavizar as retas! (Não utilizado, efetivamente um PD)



/*

Melhores runs:


kp 1.2 kd 5.0 BS= 80


kp 2 kd 10 bs= 100;



*/

int cnt=0;
int currentBaseSpeed = 80;
bool marcadores[16] = {false,false,false,false,false,false,false,false,
                       false,false,false,false,false,false,false,false};
bool brancoDetectado = false;


  void setup()
  {
  // Liga os LEDs IR dos sensores
  pinMode(LEDON, OUTPUT);
  digitalWrite(LEDON, HIGH);

  // Define os pinos dos sensores
  qtr.setTypeRC(); //Definição dos sensores principais
  Perifericos.setTypeRC(); //Definição dos sensores periféricos
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount1); //declaração dos pinos dos sensores principais
  Perifericos.setSensorPins((const uint8_t[]){D1_2, D2_2}, SensorCount2); //declaração dos pinos dos sensores periféricos

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


  void loop(){

    uint16_t position = qtr.readLineWhite(sensorValues1);
  
    // Calcula o erro em relação ao centro (3500)
    int error = map(position, 0, 7000, -100, 100);  
  
  
    int derivative = error - lastError;
  
    int normal = abs(error) / 100;
    currentBaseSpeed *= (1-normal); // Base reduzida proporcionalmente ao erro
    currentBaseSpeed = constrain(currentBaseSpeed, 40, maxSpeed); 
  
    // Correção com PID (PD)
    int correction = KP * error + KD * derivative + KI * ((error + lastError) / 2);
  
    // Calcula as velocidades dos motores usando a currentBaseSpeed
    int leftSpeed = currentBaseSpeed + (correction);
    int rightSpeed = currentBaseSpeed - (correction);


  
    // Liga os motores com as velocidades ajustadas
    //MotorE.ligar_motor(1, rightSpeed);  // Assumindo que '1' é para frente
    //MotorD.ligar_motor(1, leftSpeed); // Assumindo que '1' é para frente
  
    // Atualiza o erro anterior
    lastError = error;
  
    // Imprime dados para depuração")
    Serial.print(" | BaseSpd: "); // Nova informação para depuração
    Serial.print(currentBaseSpeed);
    Serial.print(" | VE: ");
    Serial.print(leftSpeed);
    Serial.print(" | VD: ");
    Serial.println(rightSpeed);
    Serial.print(" | cnt value: ");
    Serial.println(cnt);

    //Faz a leitura dos sensores periféricos, se ele perceber a leitura de um ele somará no contador

    Perifericos.read(sensorValues2, QTRReadMode::On);

    Perifericos.read(sensorValues2, QTRReadMode::On);

  for (int i = 0; i < SensorCount2; i++) {
    if (sensorValues2[i] < 300) {
      brancoDetectado = true;
    }

    else if (brancoDetectado && sensorValues2[i] >= 2500) {
      cnt++;
      brancoDetectado = false;
    }
    }

  // Printa os valores dos sensores D1_2 e D2_2
  Serial.print("Leitura D1_2: ");
  Serial.println(sensorValues2[0]); // D1_2 está no índice 0
  Serial.print("Leitura D2_2: ");
  Serial.println(sensorValues2[1]); // D2_2 está no índice 1

    
    // Quando detectar a primeira marca, reduz a velocidade uma vez
  if (!marcadores[0] && cnt == 1) {
    marcador[0]= true;
    currentBaseSpeed -= 20;
    Serial.println(">> Marca 1 detectada: Reduzindo velocidade.");
  }

  // Quando detectar a segunda marca, aumenta a velocidade uma vez
  if (!marcadores[1] && cnt == 2) {
    marcadores[1] = true;
    currentBaseSpeed -= 20;
    Serial.println(">> Marca 2 detectada: Aumentando velocidade.");
  }

  if(!marcadores[2] && cnt == 3) {
    marcadores[2]= true;
    currentBaseSpeed =currentBaseSpeed; 
    Serial.println(">> Marca 3 detectada: Aumentando velocidade.");
  }

  if(!marcadores[3] && cnt == 4) {
    marcadores[3] = true;
    currentBaseSpeed +=40; 
    Serial.println(">> Marca 4 detectada: Aumentando velocidade.");
  }

  if(!marcadores[4] && cnt == 5) {
    marcadores[4] = true;
    currentBaseSpeed -=30; 
    Serial.println(">> Marca 5 detectada: Aumentando velocidade.");
  }

  if(!marcadores[5] && cnt == 6) {
    marcadores[5] = true;
    currentBaseSpeed +=20; 
    Serial.println(">> Marca 6 detectada: Aumentando velocidade.");
  }

  if(!marcadores[6] && cnt == 7) {
    marcadores[6] = true;
    currentBaseSpeed =currentBaseSpeed; 
    Serial.println(">> Marca 7 detectada: Aumentando velocidade.");
  }

  if(!marcadores[7] && cnt == 8) {
    marcadores[7] = true;
    currentBaseSpeed =currentBaseSpeed; 
    Serial.println(">> Marca 8 detectada: Aumentando velocidade.");
  }

  if(!marcadores[8] && cnt == 9) {
    marcadores[8] = true;
    currentBaseSpeed =currentBaseSpeed; 
    Serial.println(">> Marca 9 detectada: Aumentando velocidade.");
  }

  if(!marcadores[9] && cnt == 10) {
    marcadores[9] = true;
    currentBaseSpeed -=20; 
    Serial.println(">> Marca 10 detectada: Aumentando velocidade.");
  }

  if(!marcadores[10] && cnt ==11) {
    marcadores[10] = true;
    currentBaseSpeed =currentBaseSpeed; 
    Serial.println(">> Marca 11 detectada: Aumentando velocidade.");
  }

  if(!marcadores[11] && cnt == 12) {
    marcadores[11] = true;
    currentBaseSpeed =currentBaseSpeed; 
    Serial.println(">> Marca 12 detectada: Aumentando velocidade.");
  }

  if(!marcadores[12] && cnt == 13) {
    marcadores[12] = true;
    currentBaseSpeed +=30 
    Serial.println(">> Marca 13 detectada: Aumentando velocidade.");
  }

  if(!marcadores[13] && cnt == 14) {
    marcadores[13] = true;
    currentBaseSpeed =currentBaseSpeed; 
    Serial.println(">> Marca 14 detectada: Aumentando velocidade.");
  }

  if(!marcadores[14] && cnt == 15) {
    marcadores[14] = true;
    currentBaseSpeed -=20; 
    Serial.println(">> Marca 15 detectada: Aumentando velocidade.");
  }

  if(!marcadores[15] && cnt == 16) {
    marcadores[15] = true;
    currentBaseSpeed +=20; 
    Serial.println(">> Marca 16 detectada: Aumentando velocidade.");
  }


  delay(10); // Pequeno delay para estabilidade e leitura serial
  }