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

  //fabs(rps*60)<1?posi+=dir:(rps*60)>1?posi++:posi--;  

  if(fabs(rps * 60) < 1) 
      posi += dir;
  else if ((rps * 60) > 1) 
      posi++;
  else 
      posi--;

}

// Função para resetar o encoder do motor
void MotorDC::resetar_encoder()
{
  eprev = 0;
  eintegral = 0;
  voltas = 0;
}

void MotorDC::andar_reto(int velocidade_rpm){

  // atualizar_tempo();

  rpm_referencia = velocidade_rpm; // Velocidade de referência

  volatile double posi_atual = 0;      // posição atual do encoder
  noInterrupts();              // desabilita interrupções
  posi_atual = posi;          // atualiza a posição atual do encoder
  interrupts();               // reabilita interrupções

  voltas_anterior = voltas; // atualiza o número de voltas anterior

  voltas = posi_atual / encoder_volta;            // calcula o número de voltas do motor
  rps = (voltas - voltas_anterior) / dt; // calcula a velocidade do motor em rps

  double e = rpm_referencia - (rps * 60); // calcula o erro da velocidade em rpm

  float p = kp * e;

  eintegral += e;

  float i = ki * eintegral*dt;

  float d = kd * ((e - eprev) / dt);

  float u = p + i + d; //p + (ki * eintegral*dt) + d;

  float pwmVal = fabs(u); // valor do pwm que será enviado ao motor

  if (pwmVal > 255) // Limita o valor do pwm para 255
  {
    pwmVal = 255;
  }

  // Define a direção do motor com base no valor de u
  if (u > 0)
  {
    dir = 1;
  }
  else if (u < 0)
  {
    dir = -1;
  }
  else
  {
    dir = 0;
  }

  if (velocidade_rpm != 0) {
    ligar_motor(dir, pwmVal);
  } else {
    ligar_motor(0, 0);
  }

  eprev = e;
  
}