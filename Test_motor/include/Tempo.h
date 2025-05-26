// Tempo.h
#ifndef TEMPO_H
#define TEMPO_H

#include <Arduino.h>

class Tempo {
  private:
    unsigned long prevMicros;
    double dt; // em segundos

  public:
    Tempo();                 // Construtor
    void atualizar();        // Atualiza o tempo atual e calcula o dt
    double getDeltaTime();   // Retorna o dt atual
};

#endif
