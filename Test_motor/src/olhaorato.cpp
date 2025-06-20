/*#include <Arduino.h>
#include <ESP32Encoder.h>
#include <variables.h>
#include <QTRSensors.h>
#include <Adafruit_NeoPixel.h>
#include <BluetoothSerial.h>
#include <ESP32Servo.h>

#include <cmath>
#include <vector>
#include <iostream>
// #include "FS.h"
// #include <LittleFS.h>
#include <string>
#include <iostream>
#include <fstream>
#include "SPIFFS.h"


using namespace std;

class Map_Data{
public:
  float leftEncoderCount;
  float rightEncoderCount;
  float meanEncoderCount;
  float leftEncoderDelta;
  float rightEncoderDelta;
  float meanEncoderDelta;
  float curveSpeed;
  float lineSpeed;
  int curve;
  float accelerationSpace;
  float desaccelerationSpace;
  float accelerationCount;
  float desaccelerationCount;

  Map_Data(float leftEncoderCount = 0, float rightEncoderCount = 0, float meanEncoderCount = 0,
              float leftEncoderDelta = 0, float rightEncoderDelta = 0, float meanEncoderDelta = 0, float curveSpeed = 0,
              float lineSpeed = 0, int curve = 0, float accelerationSpace = 0,
              float desaccelerationSpace = 0, float accelerationCount = 0, float desaccelerationCount = 0)
      : leftEncoderCount(leftEncoderCount), rightEncoderCount(rightEncoderCount), meanEncoderCount(meanEncoderCount),
      leftEncoderDelta(leftEncoderDelta), rightEncoderDelta(rightEncoderDelta), meanEncoderDelta((leftEncoderDelta + rightEncoderDelta)/2), curveSpeed(curveSpeed),
      lineSpeed(lineSpeed), curve(curve), accelerationSpace(accelerationSpace), desaccelerationSpace(desaccelerationSpace), accelerationCount(accelerationCount), desaccelerationCount(desaccelerationCount){}
};

std::vector<Map_Data> mapDataListManual;
std::vector<Map_Data> mapDataListSensor;
std::vector<Map_Data> mapDataList;

Servo Brushless;
ESP32Encoder encoder;
ESP32Encoder encoder2;
QTRSensors sArray;
BluetoothSerial SerialBT;

bool FirstTimeOnSwitchCase = true;

long int calculate_rpm_esq();
long int calculate_rpm_dir();


void ler_sensores()
{
  uint16_t sArraychannels[sArray.getSensorCount()];
  erro_sensores = sArray.readLineWhite(sArraychannels) - 3500;
  erro_f = -1 * erro_sensores;
}

void calcula_PID(float KpParam, float KdParam)
{
  P = erro_f;
  D = erro_f - erro_anterior;
  PID = (KpParam * P) + (KdParam * D);
  erro_anterior = erro_f;
}

float rotacionalErrorBuffer[9]; // buffer to store the last 5 values of translationalError
int rotacionalErrorIndex = 0; // index to keep track of the current position in the buffer
void calcula_PID_rot(float KpParamRot , float KdParamRot , float KiParamRot){
  rotacionalError = (calculate_rpm_esq() - calculate_rpm_dir());
  P_rot = rotacionalError;
  D_rot = rotacionalError - lastRotacionalError;
  
  // update the buffer and calculate the sum of the last 5 values
  // rotacionalErrorBuffer[rotacionalErrorIndex] = rotacionalError;
  // rotacionalErrorIndex = (rotacionalErrorIndex + 1) % 9;
  // float sum = 0;
  // for (int i = 0; i < 9; i++) {
  //   sum += rotacionalErrorBuffer[i];
  // }
  // I_Translacional = sum;
  PIDrot =(KpParamRot*P_rot)+(KdParamRot*D_rot);
  lastRotacionalError = rotacionalError;
}

float curva_acel(float pwm_goal)
{

  if (pwm_goal > last_pwm)
  {
    run_pwm = pwm_goal;
  }
  else if (pwm_goal < last_pwm)
  {
    last_pwm -= 0.02f;
    run_pwm = pwm_goal;
  }
  else
  {
    run_pwm = pwm_goal;
  }
  return run_pwm;
}

float translacionalErrorBuffer[9]; // buffer to store the last 5 values of translationalError
int translacionalErrorIndex = 0; // index to keep track of the current position in the buffer


void calcula_PID_translacional(float KpParam_Translacional, float KdParam_Translacional,float KiParam_Translacional, float desiredSpeed)
{
  translacionalError = curva_acel(desiredSpeed) - robotSpeed;
  P_Translacional = translacionalError;
  D_Translacional = translacionalError - lastTranslacionalError;

  // update the buffer and calculate the sum of the last 5 values
  translacionalErrorBuffer[translacionalErrorIndex] = translacionalError;
  translacionalErrorIndex = (translacionalErrorIndex + 1) % 9;
  float sum = 0;
  for (int i = 0; i < 9; i++) {
    sum += translacionalErrorBuffer[i];
  }
  I_Translacional = sum;

  PIDTranslacional = (KpParam_Translacional * P_Translacional) + (KdParam_Translacional * D_Translacional) + (KiParam_Translacional * I_Translacional);
  lastTranslacionalError = translacionalError;
}

void motorControl()
{
  velesq = PIDTranslacional + PID;
  veldir = PIDTranslacional - PID;

  if(veldir >= 0)
  {
    if(veldir > MAX_PWM) veldir = MAX_PWM;
    analogWrite(in_dir1,LOW);
    analogWrite(in_dir2,veldir);
  }
  else
  {
    veldir = (-1) * veldir;
    analogWrite(in_dir1,veldir);
    analogWrite(in_dir2,LOW);
  }

  if(velesq >= 0)
  {
    if (velesq > MAX_PWM) velesq = MAX_PWM;
    analogWrite(in_esq1,LOW);
    analogWrite(in_esq2,velesq);
  }
  else
  {
    velesq = (-1) * velesq;
    analogWrite(in_esq1,velesq);
    analogWrite(in_esq2,LOW);
  }
}

void motorControlOnLine()
{
  velesq = PIDTranslacional  + PIDrot;
  veldir = PIDTranslacional  - PIDrot;

  if(veldir >= 0)
  {
    if(veldir > MAX_PWM) veldir = MAX_PWM;
    analogWrite(in_dir1,LOW);
    analogWrite(in_dir2,veldir);
  }
  else
  {
    veldir = (-1) * veldir;
    analogWrite(in_dir1,veldir);
    analogWrite(in_dir2,LOW);
  }

  if(velesq >= 0)
  {
    if (velesq > MAX_PWM) velesq = MAX_PWM;
    analogWrite(in_esq1,LOW);
    analogWrite(in_esq2,velesq);
  }
  else
  {
    velesq = (-1) * velesq;
    analogWrite(in_esq1,velesq);
    analogWrite(in_esq2,LOW);
  }
}

void calculateRobotSpeed(void *parameter) //m/s
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(true)
  {
    leftEncoderPulse = encoder.getCount();
    rightEncoderPulse = encoder2.getCount();
    vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(SAMPLING_TIME));  // Pausa de 10ms entre as verificações
    leftDistanceTravelled = encoder.getCount() - leftEncoderPulse;
    rightDistanceTravelled = encoder2.getCount() - rightEncoderPulse;
    robotSpeed = ((leftDistanceTravelled + rightDistanceTravelled)/2)* MM_PER_COUNT / SAMPLING_TIME; //m/s
  }
}

void tratamento()
{
  if(!mapDataList.empty()) //verifica se a lista não está vazia
  {
    for(size_t i = 0; i < mapDataList.size(); i++) //percorre a lista
    {
      Map_Data& currentData = mapDataList[i]; //pega o endereço do objeto do tipo Map_Data atual a lista
      if(i == 0) //caso seja o primeiro item da lista, a variação de espaço vai ser igual ao quanto o robô andou até agora
      {
        currentData.leftEncoderDelta = currentData.leftEncoderCount;
        currentData.rightEncoderDelta = currentData.rightEncoderCount;
      }
      else //se não for o primeiro da lista, a variação de espaço é igual ao quanto ele andou até agora menos o quanto ele andou até a marcação anterior
      {
        Map_Data& previousData = mapDataList[i-1];
        currentData.leftEncoderDelta = currentData.leftEncoderCount - previousData.leftEncoderCount;
        currentData.rightEncoderDelta = currentData.rightEncoderCount - previousData.rightEncoderCount;
      }
      currentData.meanEncoderCount = (currentData.leftEncoderCount + currentData.rightEncoderCount)/2;
      
      //transforma o delta pulsos para delta em metros
      float leftEncoderDeltaMeter = (MM_PER_COUNT * currentData.leftEncoderDelta)/1000;
      float rightEncoderDeltaMeter = (MM_PER_COUNT * currentData.rightEncoderDelta)/1000;
      float meanEncoderDeltaMeter = (leftEncoderDeltaMeter + rightEncoderDeltaMeter)/2;

      currentData.meanEncoderDelta = meanEncoderDeltaMeter;

      if(leftEncoderDeltaMeter == rightEncoderDeltaMeter) //verificação para que não haja divisão por zero no cálculo de curveRadius
      {
        currentData.curve = 0;
      }
      else
      {
        //calcula o raio da curva
        float curveRadius = abs((DISTANCEWHEELTOCENTER/2) * ((leftEncoderDeltaMeter+rightEncoderDeltaMeter)/(leftEncoderDeltaMeter-rightEncoderDeltaMeter)));

        if(curveRadius <= 0.5) //se o raio da curva for menor ou igual do que 50cm(0.5m), então é uma curva 
        {
          currentData.curve = 1;
          currentData.curveSpeed = pow((curveRadius*FRICTION)*(GRAVITY+(BRUSHLESSFORCE/MASS)),0.5); //calcula a velocidade para fazer a curva
        }
        else //se for maior é uma reta
        {
          currentData.curve = 0;
        }
      }
    }
    for(short i = 0; i < mapDataList.size(); i++)
    {
      // SerialBT.print(mapDataList[i].meanEncoderCount);SerialBT.print("\t");
      // SerialBT.print(mapDataList[i].meanEncoderDelta);SerialBT.print("\t");
      // SerialBT.print(mapDataList[i].curveSpeed);SerialBT.print("\t");
      //SerialBT.println(mapDataList[i].curve);
    }
  }
  else
  {
    SerialBT.println("A lista está vazia");
  }

  float accelerationSpaceMeter;
  float desaccelerationSpaceMeter;
  for(int i = 0; i < mapDataList.size(); i++)
  {
    if(mapDataList[i].curve == 0) //entra se for uma reta
    {
      if (i != mapDataList.size()-1) //entra se não for o ultimo item da lista
      {
        if(i != 0) //entra se não for o primiero item da lista
        {
          //calcula o espaço para aceleração e desaceleração
          accelerationSpaceMeter = (pow(MAXSPEED,2) - pow(mapDataList[i-1].curveSpeed,2))/(2*acceleration);
          desaccelerationSpaceMeter = -((pow(mapDataList[i+1].curveSpeed,2) - pow(MAXSPEED,2))/(2*acceleration));

          //transforma metros em pulsos de encoder
          mapDataList[i].accelerationSpace = (accelerationSpaceMeter/MM_PER_COUNT)*1000;
          mapDataList[i].desaccelerationSpace = (desaccelerationSpaceMeter/MM_PER_COUNT)*1000;
        }
        else //entra se for o primeiro item da lista
        {
          //calcula o espaço para aceleração e desaceleração
          accelerationSpaceMeter = (pow(MAXSPEED,2))/(2*acceleration);
          desaccelerationSpaceMeter = -((pow(mapDataList[i+1].curveSpeed,2) - pow(MAXSPEED,2))/(2*acceleration));

          //transforma metros em pulsos de encoder
          mapDataList[i].accelerationSpace = (accelerationSpaceMeter/MM_PER_COUNT)*1000;
          mapDataList[i].desaccelerationSpace = (desaccelerationSpaceMeter/MM_PER_COUNT)*1000;
        }
      }
      else //se for o ultimo item da lista
      {
        //calcula o espaço para aceleração e desaceleração
        accelerationSpaceMeter = (pow(MAXSPEED,2) - pow(mapDataList[i-1].curveSpeed,2))/(2*acceleration);
        desaccelerationSpaceMeter = -((pow(2,2) - pow(MAXSPEED,2))/(2*acceleration));

        //transforma metros em pulsos de encoder
        mapDataList[i].accelerationSpace = (accelerationSpaceMeter/MM_PER_COUNT)*1000.0f;
        mapDataList[i].desaccelerationSpace = (desaccelerationSpaceMeter/MM_PER_COUNT)*1000.0f;
      }    
    }
    if(i==0)
    {
      if((accelerationSpaceMeter + desaccelerationSpaceMeter) < mapDataList[i].meanEncoderDelta) //se a soma dos espaços for menor do que o tamanho da reta
      {
        mapDataList[i].accelerationCount = mapDataList[i].accelerationSpace;
        mapDataList[i].desaccelerationCount = mapDataList[i].meanEncoderCount - mapDataList[i].desaccelerationSpace;
      }
      else
      {
        mapDataList[i].accelerationCount = (((mapDataList[i].meanEncoderDelta/MM_PER_COUNT)*1000)/2);
        mapDataList[i].desaccelerationCount = mapDataList[i].meanEncoderCount - (((mapDataList[i].meanEncoderDelta/MM_PER_COUNT)*1000)/2);
      }
    }
    else
    {
      if((accelerationSpaceMeter + desaccelerationSpaceMeter) < mapDataList[i].meanEncoderDelta) //se a soma dos espaços for menor do que o tamanho da reta
      {
        mapDataList[i].accelerationCount = mapDataList[i-1].meanEncoderCount + mapDataList[i].accelerationSpace;
        mapDataList[i].desaccelerationCount = mapDataList[i].meanEncoderCount - mapDataList[i].desaccelerationSpace;
      }
      else
      {
        mapDataList[i].accelerationCount = (((mapDataList[i].meanEncoderDelta/MM_PER_COUNT)*1000)/2) + mapDataList[i-1].meanEncoderCount;
        mapDataList[i].desaccelerationCount = mapDataList[i].meanEncoderCount - (((mapDataList[i].meanEncoderDelta/MM_PER_COUNT)*1000)/2);
      }
    }
  }
}

void hybridMapping(const std::vector<Map_Data>& manualData, const std::vector<Map_Data>& sensorData, std::vector<Map_Data>& resultData) {
  float thresholdSort = 2000;
  resultData.resize(manualData.size());
  for (std::size_t i = 0; i < manualData.size(); ++i) {
    bool found = false;
    for (std::size_t j = 0; j < sensorData.size(); ++j) {
      if (std::abs (manualData[i].meanEncoderCount - sensorData[j].meanEncoderCount) <= thresholdSort){
        resultData[i] = sensorData[j];
        found = true;
        break;
      }
    }
    if(!found){
      resultData[i] = manualData[i];
    }
  }
}



long int calculate_rpm_esq(){
  Serial.print(enc_esq_pul);
  Serial.print(", ");
  Serial.println(pul_prev_esq);
  enc_esq_pul = encoder.getCount() - pul_prev_esq;
  pul_prev_esq = encoder.getCount();

  return enc_esq_pul;
}

long int calculate_rpm_dir(){
  Serial.print(enc_dir_pul);
  Serial.print(", ");
  Serial.println(pul_prev_dir);
  enc_dir_pul =  encoder2.getCount() - pul_prev_dir;
  pul_prev_dir = encoder2.getCount();
  return enc_dir_pul;
}

void setup()
{
  Serial.begin(115200);

  pinMode(in_esq1, OUTPUT);
  pinMode(in_esq2, OUTPUT);
  pinMode(in_dir1, OUTPUT);
  pinMode(in_dir2, OUTPUT);
  pinMode(PWM_RIGHT, OUTPUT);
  pinMode(PWM_LEFT, OUTPUT);
  pinMode(stby, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(s_lat_esq, INPUT);
  pinMode(s_lat_dir, INPUT);
  pinMode(BRUSHLESS_PIN, OUTPUT);
  pinMode(boot, INPUT);

  led_stip.begin();
  SerialBT.begin("Semreh"); //Bluetooth device name

  ESP32Encoder::useInternalWeakPullResistors = UP;

  encoder.attachFullQuad(enc_eq_A, enc_eq_B);
  encoder2.attachFullQuad(enc_dir_B, enc_dir_A);

  encoder.clearCount();
  encoder2.clearCount();

  Brushless.attach(BRUSHLESS_PIN, 1000, 2000);

  Brushless.write(180);
  delay(5000);
  Brushless.write(0);

  if(!SPIFFS.begin(true)){
        SerialBT.println("SPIFFS Mount Failed");
        return;
  }

  sArray.setTypeMCP3008();
  sArray.setSensorPins((const uint8_t[]){0, 1, 2, 3, 4, 5, 6, 7}, 8, (gpio_num_t)out_s_front, (gpio_num_t)in_s_front, (gpio_num_t)clk, (gpio_num_t)cs_s_front, 1350000, VSPI_HOST);
  sArray.setSamplesPerSensor(5); // VERIFICARRR

  for (int i = 0; i < 200; i++)
  {
    if(i < 100)
    {
      led_stip.setPixelColor(0,B);
      led_stip.show();
    }

    else
    {
      led_stip.setPixelColor(0,R);
      led_stip.show();
    }
    sArray.calibrate();
    delay(20);
  }
  led_stip.setPixelColor(0,0,0,0);
  led_stip.show();
  encoder.clearCount();
  encoder2.clearCount();

  xTaskCreatePinnedToCore(calculateRobotSpeed,"Velocidade",10000,NULL,1,NULL,1);
  //xTaskCreatePinnedToCore(ler_laterais,"Sensores Laterais",4000,NULL,1,NULL,0);
}

*/