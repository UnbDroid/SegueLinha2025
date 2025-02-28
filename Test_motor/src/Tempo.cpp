#include "Tempo.h"

extern unsigned long T;
extern unsigned long prevT;
extern double dt;


void atualizar_tempo()
{
    T = micros();
    dt = ((float) (T - prevT))/( 1.0e6 );
    prevT = T;
}