#include "MotorDC.h"
#include "Arduino.h"
#include "Wire.h"
#include "Tempo.h"

//* Esse arquivo contém a implementação da classe MotorDC, que é responsável por controlar o motor DC do robô
//* e fornecer os valores de velocidade e direção de giro do motor


MotorDC::MotorDC(const int ENCA, const int ENCB, const int IN1, const int IN2)
{
    this->ENCA = ENCA;
    this->ENCB = ENCB;
    this->IN1 = IN1;
    this->IN2 = IN2;
    pinMode(ENCA, INPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    

}

void MotorDC::configurar(int ticks_por_volta, float kp, float ki, float kd){
    this -> encoder_volta = ticks_por_volta;
    this -> kp = kp;
    this -> ki = ki;
    this -> kd = kd;
}
void MotorDC::parar(){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
}


void MotorDC::ligar_motor(int direcao, int pwmVal){

    if (direcao == 1){
      // 1 para frente
      analogWrite(IN1, pwmVal);
      analogWrite(IN2, 0);
    }
    else if (direcao == -1){
      // -1 para trás
      analogWrite(IN1, 0);
      analogWrite(IN2, pwmVal);
    }
    else{ // 0 para parar
      digitalWrite(IN1, 255);
      analogWrite(IN2, 255);
    }

}

// Função para ler o encoder do motor
void MotorDC::ler_encoder(){
  posi++;
  
}

// Função para resetar o encoder do motor
void MotorDC::resetar_encoder()
{
  eprev = 0;
  eintegral = 0;
  voltas = 0;
}
void MotorDC::set_RPM(int velocidade_rpm){

  //Serial.print("encoder_volta: "); Serial.println(encoder_volta);

  tempo.atualizar();
  double dt = tempo.getDeltaTime();

  rpm_referencia = fabs(velocidade_rpm);

  volatile double posi_atual = 0;
  noInterrupts();
  posi_atual = posi;
  interrupts();

  voltas_anterior = voltas;
  voltas = posi_atual / encoder_volta;
  rps = (voltas - voltas_anterior) / dt;
  rps = rps/100;

  double rpm = rps * 60;
  double e = rpm_referencia - rpm;

  float p = kp * e;
  eintegral += e;
  float i = ki * eintegral * dt;
  float d = kd * (e - eprev) / dt;
  float u = p + i + d;

  float pwmVal = constrain(fabs(u), 0, 255);

  if (velocidade_rpm > 0) dir = 1;
  else if (velocidade_rpm < 0) dir = -1;
  else dir = 0;

  ligar_motor(dir, pwmVal);
  eprev = e;

}
