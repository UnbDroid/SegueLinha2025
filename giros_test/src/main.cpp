#include <Arduino.h>
#include <MPU9250.h>

// Defina os pinos que você vai usar
const int pinSDA = 18; // Pino SDA
const int pinSCL = 19; // Pino SCL

// Configurar o objeto MPU9250 com os pinos definidos
MPU9250 IMU(Wire, 0x68);
int status;

void setup() {
  // Configurar os pinos I2C
  Wire.begin(pinSDA, pinSCL);

  // Serial para exibir dados
  Serial.begin(115200);
  while (!Serial) {}

  // Iniciar comunicação com o IMU
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1) {}
  }
}

void loop() {
  // Ler o sensor
  IMU.readSensor();
  // Exibir os dados
  Serial.print(IMU.getAccelX_mss(), 6);
  Serial.print("\t");
  Serial.print(IMU.getAccelY_mss(), 6);
  Serial.print("\t");
  Serial.print(IMU.getAccelZ_mss(), 6);
  Serial.print("\t");
  Serial.print(IMU.getGyroX_rads(), 6);
  Serial.print("\t");
  Serial.print(IMU.getGyroY_rads(), 6);
  Serial.print("\t");
  Serial.print(IMU.getGyroZ_rads(), 6);
  Serial.print("\t");
  Serial.print(IMU.getMagX_uT(), 6);
  Serial.print("\t");
  Serial.print(IMU.getMagY_uT(), 6);
  Serial.print("\t");
  Serial.print(IMU.getMagZ_uT(), 6);
  Serial.print("\t");
  Serial.println(IMU.getTemperature_C(), 6);
  delay(1000);
}
//alteração
