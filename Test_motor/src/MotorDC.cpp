#include "MotorDC.h"
#include "Arduino.h"



MotorDC::MotorDC(const int ENCA, const int ENCB, const int IN1, const int IN2) 
    : ENCB(ENCB), IN1(IN1), IN2(IN2) {
    pinMode(ENCB, INPUT_PULLUP);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

}



void MotorDC::configurar(int ticks_por_volta, float kp, float ki, float kd) {
    encoder_volta = ticks_por_volta;
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

void MotorDC::parar() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
}

void MotorDC::ligar_motor(int direcao, int pwmVal) {
    pwmVal = constrain(pwmVal, 0, 255);
    if (direcao == 1) {
        analogWrite(IN1, pwmVal);
        digitalWrite(IN2, LOW);
    } 
    else if (direcao == -1) {
        digitalWrite(IN1, LOW);
        analogWrite(IN2, pwmVal);
    } 
    else {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
    }
}

// Single-channel encoder counting
void MotorDC::ler_encoder() {
    posi++;  // Count every rising edge on ENCB
}

void MotorDC::resetar_encoder() {
    noInterrupts();
    posi = 0;
    interrupts();
    eprev = 0;
    eintegral = 0;
    voltas = 0;
    voltas_anterior = 0;
}

void MotorDC::set_RPM(int velocidade_rpm) {
    tempo.atualizar();
    double dt = tempo.getDeltaTime();
    
    rpm_referencia = fabs(velocidade_rpm);
    double posi_atual = get_pulses();  // Safe read
    
    voltas_anterior = voltas;
    voltas = posi_atual / encoder_volta;
    rps = (voltas - voltas_anterior) / dt;
    
    double e = rpm_referencia - (rps * 60);
    
    // PID calculation with anti-windup
    float p = kp * e;
    eintegral = constrain(eintegral + e * dt, -1000, 1000);
    float i = ki * eintegral;
    float d = kd * (e - eprev) / dt;
    float u = p + i + d;
    
    int pwmVal = constrain(static_cast<int>(fabs(u)), 0, 255);
    
    dir = (velocidade_rpm > 0) ? 1 : ((velocidade_rpm < 0) ? -1 : 0);
    ligar_motor(dir, pwmVal);
    eprev = e;
}