#include <Arduino.h>

const int pinLDR[7] = {A0, A1, A2, A3, A4, A5, A6};

const int threshold = 500; // Ajuste conforme necessário

void setup()
{

  Serial.begin(9600);
  while (!Serial)
  {
  }

  Serial.println("Iniciando sensores de luz...");
}

void loop()
{

  String output = "";

  for (int i = 0; i < 7; i++)
  {
    int lightValue = analogRead(pinLDR[i]);

    if (lightValue < threshold)
    {
      output += "P";
    }
    else
    {
      output += "B";
    }

    if (i < 6)
    {
      output += ",";
    }
  }

  Serial.println(output);

  delay(100);
