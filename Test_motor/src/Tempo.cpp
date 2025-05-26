// Tempo.cpp
#include "Tempo.h"

Tempo::Tempo() {
  prevMicros = micros();
  dt = 0.0;
}

void Tempo::atualizar() {
  unsigned long currentMicros = micros();
  dt = (currentMicros - prevMicros) / 1.0e6; // converte para segundos
  prevMicros = currentMicros;
}

double Tempo::getDeltaTime() {
  return dt;
}
