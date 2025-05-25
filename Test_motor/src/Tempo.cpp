#include "Tempo.h"

unsigned long T;
unsigned long prevT;
double dt;


void atualizar_tempo()
{
    T = micros();
    dt = ((float) (T - prevT))/( 1.0e6 );
    prevT = T;
}