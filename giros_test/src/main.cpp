#include <Arduino.h>
#include <MPU9250.h>

// Defina os pinos que você vai usar
const int pinSDA = 21; // Pino SDA
const int pinSCL = 22; // Pino SCL

// Configurar o objeto MPU9250 com os pinos definidos
MPU9250 IMU(Wire, 0x68);
int status;

// Variáveis para armazenar os ângulos integrados
float angleX = 0;
float angleY = 0;
float angleZ = 0;

// Variável para armazenar o tempo anterior
unsigned long prevTime = 0;

void setup() {
  // Configurar os pinos I2C
  Wire.begin(pinSDA, pinSCL);

  // Serial para exibir dados
  Serial.begin(9600);
  while (!Serial) {}

  Serial.println("Iniciando...");

  // Iniciar comunicação com o IMU
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1) {}
  } else {
    Serial.println("IMU initialized successfully");
  }

  // Inicializar o tempo anterior
  prevTime = millis();
}

void loop() {
  // Ler o sensor
  IMU.readSensor();

  // Calcular o deltaTime
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - prevTime) / 1000.0; // Converter para segundos
  prevTime = currentTime;

  // Integrar os valores do giroscópio para calcular os ângulos
  angleX += IMU.getGyroX_rads() * deltaTime;
  angleY += IMU.getGyroY_rads() * deltaTime;
  angleZ += IMU.getGyroZ_rads() * deltaTime;

  // Converter os ângulos de radianos para graus
  float angleX_deg = angleX * 180.0 / PI;
  float angleY_deg = angleY * 180.0 / PI;
  float angleZ_deg = angleZ * 180.0 / PI;

  // Exibir os ângulos integrados em graus
  Serial.print("AngleX: ");
  Serial.print(angleX_deg, 6);
  Serial.print("\tAngleY: ");
  Serial.print(angleY_deg, 6);
  Serial.print("\tAngleZ: ");
  Serial.println(angleZ_deg, 6);

  delay(100);
}
