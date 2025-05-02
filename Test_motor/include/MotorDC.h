#ifndef MotorDC_h
#define MotorDC_h
#pragma once
#include "Arduino.h"
#include "Tempo.h"

//* Este arquivo contém a declaração da classe MotorDC, que é responsável por
//* controlar o motor DC e fornecer os valores de velocidade e direção de giro do motor

class MotorDC{
    public:
        MotorDC(const int ENCA, const int ENCB, const int IN1, const int IN2); // Construtor da classe MotorDC
        void parar();
        void configurar(int ticks_por_volta, float kp, float ki, float kd); // Função para configurar o motor
        void ligar_motor(int direcao, int pwmVal);
        void ler_encoder();
        void resetar_encoder();
        void andar_reto(int velocidade_rpm);
        volatile double posi; // posição do motor em ticks do encoder
        double rps = 0; // velocidade ATUAL do motor em rotações por segundo
        int encoder_volta; // valor de encoder referente a uma volta completa da roda
        double comprimento_roda = 2 * M_PI * 6.272; //TODO: medir o raio da roda real

    private:
        int ENCA; // Cabo amarelo
        int ENCB; // Cabo branco
        int PWM;
        int IN1;
        int IN2;
        double voltas = 0; // número de voltas do motor
        double voltas_anterior = 0; // número de voltas do motor no instante anterior, para cálculo do erro
        float kp; // constante proporcional do controle PID
        float ki; // constante integral do controle PID
        float kd; // constante derivativa do controle PID
        int rpm_referencia; // velocidade desejada do motor, velocidade que ele buscará alcançar
        double rpm_max = 87; // velocidade máxima do motor (apenas por curiosidade, usar caso seja necessário)
        float eprev = 0;
        float eintegral = 0; // erro acumulado pro cálculo do ki
        int dir = 1; // 1 para frente, -1 para trás (pelo menos essa é a ideia)
        
};

#endif