/*#include <Arduino.h>
#include <QTRSensors.h>
#include "MotorDC.h"
#include "Pins.h"


// Sensor object
QTRSensors qtr;
//const uint8_t SensorCount = 8;
//uint16_t sensorValues[SensorCount];

// Motor objects
MotorDC MotorE(ENCA_E, ENCB_E, IN2, IN1);
MotorDC MotorD(ENCA_D, ENCB_D, IN3, IN4);
/*
// ===== PID CONTROL SYSTEM =====
// Line Following PID
float KP_line = 2.0;       // Proportional gain for line following
float KD_line = 0.0;       // Derivative gain for line following
int lastLineError = 0;

// Rotational PID (wheel synchronization)
float KP_rot = 2.5;        // Proportional gain for rotation
float KD_rot = 0.3;        // Derivative gain for rotation
int lastRotError = 0;
long lastLeftPulse = 0;
long lastRightPulse = 0;

// Translational PID (speed control)
float KP_speed = 1.8;      // Proportional gain for speed
float KD_speed = 0.2;      // Derivative gain for speed
float KI_speed = 0.05;     // Integral gain for speed
int lastSpeedError = 0;
float speedIntegral = 0;
float speedErrorBuffer[9] = {0};
int speedErrorIndex = 0;

// Speed parameters
const int MAX_SPEED = 100;     // Maximum speed in straight
const int MIN_SPEED = 40;      // Minimum speed in curves
const int ERROR_THRESH = 30;   // Error threshold for speed reduction
const int SAMPLING_TIME = 10;  // ms

// Acceleration control
float lastPWM = 0;
const float ACCEL_RATE = 0.5;  // PWM units per cycle


*/

/*
const uint8_t SensorCount = 8;      // Número de sensores
uint16_t sensorValues[SensorCount]; // Valores dos sensores

// --- Novas Constantes para Controle de Velocidade Dinâmico ---
const int MAX_SPEED_STRAIGHT = 100;   // Velocidade máxima em retas (respeitando o limite original)
const int MIN_SPEED_CURVE = 35;       // Velocidade mínima em curvas (ajuste conforme necessário)
const int ERROR_THRESHOLD_MAX_REDUCTION = 50; // 2000 Magnitude do erro a partir da qual a velocidade estará no mínimo 
                                                // (erro máximo é ~3500, então 2000 é uma curva já considerável)
                                                // Ajuste este valor experimentalmente.

// Variáveis do controle PID
int error = 0;
int lastError = 0;
float KP = 2;   // Ganho pra curvas! 5.8
float KD = 2.8;   // Ganho para suavizar as curvas!
float KI = 0.0;   // Ganho para suavizar as retas! (Não utilizado, efetivamente um PD)
// int baseSpeed = 100; // Esta será substituída pela velocidade dinâmica



// ===== SETUP =====
void setup() {
  // Configure QTR sensor
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){D1, D2, D3, D4, D5, D6, D7, D8}, SensorCount);
  
  Serial.begin(115200);
  delay(2000);
  
  // Hardcoded calibration (replace with your actual values)
  const uint16_t minValues[SensorCount] = {741, 608, 663, 527, 521, 611, 649, 815};
  const uint16_t maxValues[SensorCount] = {2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500};
  
  qtr.calibrationOn.minimum = new uint16_t[SensorCount];
  qtr.calibrationOn.maximum = new uint16_t[SensorCount];
  qtr.calibrationOn.initialized = true;
  std::copy(minValues, minValues + 8, qtr.calibrationOn.minimum);
  std::copy(maxValues, maxValues + 8, qtr.calibrationOn.maximum);

  attachInterrupt(digitalPinToInterrupt(ENCB_D), [](){MotorD.ler_encoder();}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCB_E), [](){MotorE.ler_encoder();}, CHANGE);
  
  // Initialize motors
  MotorE.configurar(720, 1, 0.0, 0.0); 
  MotorD.configurar(720, 1, 0.0, 0.0); 
}


float smoothAcceleration(float targetPWM) {
  if (targetPWM > lastPWM) {
    // Immediate acceleration
    lastPWM = targetPWM;
  } else if (targetPWM < lastPWM) {
    // Gradual deceleration
    lastPWM = max(targetPWM, lastPWM - ACCEL_RATE);
  }
  return lastPWM;
}


int calculateRotationalPID() {
  // Get current encoder counts
  long leftPulse = MotorE.get_pulses();
  long rightPulse = MotorD.get_pulses();
  
  // Calculate pulses since last reading
  long leftDelta = leftPulse - lastLeftPulse;
  long rightDelta = rightPulse - lastRightPulse;
  
  // Store current values for next calculation
  lastLeftPulse = leftPulse;
  lastRightPulse = rightPulse;
  
  // Calculate error (difference in wheel speeds)
  int error = leftDelta - rightDelta;
  int derivative = error - lastRotError;
  lastRotError = error;
  
  return KP_rot * error + KD_rot * derivative;
}

int calculateTranslationalPID(int targetSpeed, int currentSpeed) {
  int error = targetSpeed - currentSpeed;
  int derivative = error - lastSpeedError;
  
  // Update integral term with moving average
  speedErrorBuffer[speedErrorIndex] = error;
  speedErrorIndex = (speedErrorIndex + 1) % 9;
  speedIntegral = 0;
  for (int i = 0; i < 9; i++) {
    speedIntegral += speedErrorBuffer[i];
  }
  
  lastSpeedError = error;
  
  return KP_speed * error + KD_speed * derivative + KI_speed * speedIntegral;
}

void loop() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime >= SAMPLING_TIME) {
    lastTime = currentTime;
    
    // Read sensors
    uint16_t position = qtr.readLineWhite(sensorValues);
    int lineError = (position - 3500) / 100;  // Scaled error
    
    // Small deadzone
    if(abs(lineError) < 5) {
      lineError = 0;
    }
    
    // Base speed (constant for now)
    int baseSpeed = 80;
    
    // Simple P control only
    int correction = KP_line * lineError;
    
    // Calculate motor speeds
    int leftSpeed = constrain(baseSpeed + correction, MIN_SPEED, MAX_SPEED);
    int rightSpeed = constrain(baseSpeed - correction, MIN_SPEED, MAX_SPEED);
    
    MotorE.ligar_motor(1, rightSpeed);
    MotorD.ligar_motor(1, leftSpeed);
    
    // Debug output
    Serial.print("Pos:"); Serial.print(position);
    Serial.print(" | Err:"); Serial.print(lineError);
    Serial.print(" | L:"); Serial.print(leftSpeed);
    Serial.print(" | R:"); Serial.println(rightSpeed);
  }
  delay(1);
}
  

*/