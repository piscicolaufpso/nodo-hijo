
#include<SPI.h>
#include<RF24.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#define Pin 2                        
OneWire ourWire(Pin);                
DallasTemperature sensors(&ourWire);
#define SensorPin A0           
#define Offset 0.00
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40  
int pHArray[ArrayLenth];
int pHArrayIndex=0;
long periodo = 120000;
unsigned long transcurrido = 0;
RF24 radio(9, 10);

void setup(void) {
  pinMode(LED,OUTPUT);
  sensors.begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();
}

void loop(void) {
  const char text[50] = "pecera_001";
  sensors.requestTemperatures();
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  float temp = sensors.getTempCByIndex(0);
  const char buffer[8]= " ";
  dtostrf(temp, 6, 2,buffer);
  strcat(text,"/temp:");
  strcat(text,buffer);
  
  if(millis()-samplingTime > samplingInterval){
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
  }
  if(millis() - printTime > printInterval){
      digitalWrite(LED,digitalRead(LED)^1);
      printTime=millis();
  }
  dtostrf(pHValue, 6, 2,buffer);
  strcat(text,"/ph:");
  strcat(text,buffer);

  if(millis()>transcurrido+periodo){
    radio.write(&text, sizeof(text));
    transcurrido = millis();
  }
}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){  
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;    
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;   
          max=arr[i];
        }else{
          amount+=arr[i];
        }
      }
    }
    avg = (double)amount/(number-2);
  }
  return avg;
}
