#include "uart.h"
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;
Zumo32U4ProximitySensors proxSensors;

struct data {
    int32_t axis[8];
};


uint16_t brightnessLevels[] = {1,2,4,8,16,32,54};

void setup() {
  GComm.begin(57600);
  Serial.begin(115200);
  while (!Serial) {;}
  // put your setup code here, to run once:
  Serial.println("started v0");  
  buzzer.playFrequency(440,200,15);
  proxSensors.initFrontSensor();
  proxSensors.setBrightnessLevels(brightnessLevels,7);
  delay(1000);
  buzzer.playFrequency(440,200,15);
}

char b[1024];

void loop() {
  data new_data;
  int r =  GComm.read(b,1024);
  if (r){
    b[r]=0;
    Serial.println("");
    Serial.println(b);
  }
  delay(1000);
  Serial.print(".");
  // put your main code here, to run repeatedly:

}
