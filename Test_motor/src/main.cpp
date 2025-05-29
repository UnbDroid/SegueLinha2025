#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"
#include "algorithm"

// QTRSensors objeto
QTRSensors qtr;

// OAtivando Objetos do nosso motor
MotorDC MotorE(ENCA1, ENCB1, IN2, IN1);
MotorDC MotorD(ENCA2, ENCB2, IN3, IN4); 
int maxRPM = 1000; // RPM máximo para os motores
int minRPM = 216; // RPM mínimo para os motores (isso considerando que o nosso pwm mín fosse 50)

const int MAX_SPEED_STRAIGHT = 100;           //tranformar (255)PWM ==> RPM(1000) 
const int MIN_SPEED_CURVE = 35;               //tranformar (255)PWM ==> RPM(1000) 
const int ERROR_THRESHOLD_MAX_REDUCTION = 50; //tranformar (255)PWM ==> RPM(1000)

// Variáveis do controle PID DOS SENSORES
int error = 0;
int lastError = 0;
float KPs= 2;   // Ganho pra curvas! 5.8
float KDs = 2.8;   // Ganho para suavizar as curvas!
float KIs = 0.0;   // Ganho para suavizar as retas! (Não utilizado, efetivamente um PD)
// int baseSpeed = 100; // Esta será substituída pela velocidade dinâmica

const uint8_t SensorCount = 8; // Number of sensors
uint16_t sensorValues[SensorCount]; // Array to store sensor values

//esta função lê os valores retornados dos encoderes(pode ser útil para definir o ticks por volta)
void encoder_callback() {
  MotorD.ler_encoder();
  MotorE.ler_encoder();
}

void setup()
{ 

  pinMode(LEDON, OUTPUT);
  digitalWrite(LEDON, HIGH); //não sei ainda o que fazer com isso aqui, antes ia agora nn vai.

  Serial.begin(115200);
  delay(100);
  
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);
  
  // Calibração dos sensores, se quiser pode trocar por calibração manual. Porém ainda não testei o funcionamento do código PD com isto.
  Serial.println("Calibrando sensores...");
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
    delay(10);
  }
  Serial.println("Calibração concluída.");
  Serial.println("------------------------------------");
  Serial.println("Iniciando loop principal...");

  //Ativação das portas de interrupção dos encoderes dos nossos motores.
  attachInterrupt(digitalPinToInterrupt(ENCB1), encoder_callback, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCB2), encoder_callback, RISING);

  MotorD.configurar(12.0, 1.0, 0.0, 0.0); // Constantes PID da VELOCIADE DOS MOTORES (NÃO É DOS SENSORES) PARA O MOTOR DIREITO
  MotorE.configurar(12.0, 1.0, 0.0, 0.0); // Constantes PID da VELOCIADE DOS MOTORES (NÃO É DOS SENSORES) PARA O MOTOR ESQUERDO

  //isso é opcional, é só para ter certeza de que nossos motores ligaram.
  MotorD.ligar_motor(0,0);
  MotorE.ligar_motor(0,0);
}

void loop(){

  // esta parte ativa a função de leitura de linhas pretas do nosso motor e retorna a posição de 0 a 7000
  // onde 0 é a extremidade esquerda e 7000 é a extremidade direita
  uint16_t position = qtr.readLineBlack(sensorValues);

  // ffaz a média da faixa de 0 7000 da nossa variável posicional
  error = position - 3500;

  // esta parte e responsável pela deriavada, então é a variação do erro com o tempo
  int derivative = error - lastError;

  // Calcula a velocidade base atual com base no erro
  int currentBaseSpeed;
  long errorMagnitude = abs(error);


  // Mapeia a magnitude do erro para a velocidade base, reduzindo a velocidade máxima conforme o erro aumenta
  // A velocidade base diminui linearmente de MAX_SPEED_STRAIGHT a MIN_SPEED_CURVE conforme o erro aumenta
  currentBaseSpeed = map(errorMagnitude, 0, ERROR_THRESHOLD_MAX_REDUCTION, MAX_SPEED_STRAIGHT, MIN_SPEED_CURVE);
  currentBaseSpeed = constrain(currentBaseSpeed, MIN_SPEED_CURVE, MAX_SPEED_STRAIGHT);

  //PID dos sensores com base no erro posicional
  int correction = KPs * error + KDs * derivative; // + KI * (error + lastError) / 2;

  // Regula a velociada com base no PWM o nosso objetivo e deixar isso numa descala de 0 a 1000 que é em RPM
  int leftSpeed = currentBaseSpeed + correction;
  int rightSpeed = currentBaseSpeed - correction;

  leftSpeed = constrain(leftSpeed, 0, MAX_SPEED_STRAIGHT);
  rightSpeed = constrain(rightSpeed, 0, MAX_SPEED_STRAIGHT);

  lastError = error;

  //Antes de soltar este valor na função que seta o nosso PID precisamos passar este valor de PWM ==> RPM, pois a função opera em RPM e responde en PWM.
  //Portanto, aqui está feita uma regra de três simples para Left e Right Speed. Ambas operam normalmente em PWM até chegarem aqui e serem passadas para RPM
  // isso é um teste. Lembrando que esta relação foi feita considerando 1000 como nosso rpm max possível e 255 como sendo o PWM max possível

  int leftSpeedRPM = (leftSpeed*1000)/255;
  int rightSpeedRPM = (rightSpeed*1000)/255;

  //Aplicar o leftSpeed e rightSpeed nos motores (EM RPM, esta função já converte para PWM)

  MotorD.set_RPM(rightSpeedRPM);
  MotorE.set_RPM(leftSpeedRPM);

  //o delay ajuda a funcionar, sem ele o motor não faz nada.
  delay(50);
  
}