#ifndef MotorDC_h
#define MotorDC_h
#include "Arduino.h"
#include "Tempo.h"

class MotorDC {
public:
    MotorDC(const int ENCA, const int ENCB, const int IN1, const int IN2);
    void parar();
    void configurar(int ticks_por_volta, float kp, float ki, float kd);
    void ligar_motor(int direcao, int pwmVal);
    void ler_encoder();
    void resetar_encoder();
    void set_RPM(int velocidade_rpm);
    
    inline double get_pulses() const __attribute__((always_inline)) {
        noInterrupts();
        double count = posi;
        interrupts();
        return count;
    }
    
    inline double get_rpm() const { return (rps * 60); }

private:

    Tempo tempo;
    int encoder_volta;
    const int ENCB;  // Only using this pin for encoder
    int IN1, IN2;
    volatile double posi = 0;  // Encoder position
    double rps = 0;            // Rotations per second
    double voltas = 0;
    double voltas_anterior = 0;
    float kp, ki, kd;
    int rpm_referencia;
    float eprev = 0;
    float eintegral = 0;
    int dir = 1;
};

#endif